#!/usr/bin/perl -w

use strict;

=head1

Generate a testament describing the current Git status. This gets written
out in a C form which can be used to construct the Greyhound Git testament
file for signon notification.

If there is no Git in place, the data is invented arbitrarily.

=cut

$ENV{LC_ALL} = 'C';

my $root = shift @ARGV;
my $targetfile = shift @ARGV;

my %gitinfo; # The Git information

$root .= "/" unless ($root =~ m@/$@);

my $git_present = 0;
if ( -d ".git" ) {
   $git_present = 1;
}

sub compat_tmpnam {
   # File::Temp was introduced in Perl 5.6.1
   my $have_file_tmp = eval { require File::Temp };

   if ( ! $have_file_tmp ) {
     return "$$.gitt";
   } else {
     return "$$.gitt";#File::Temp::tmpnam();
   }
}

sub compat_md5_hex {
   # Digest::MD5 was introduced in Perl 5.7.1
   my $have_digest_md5 = eval { require Digest::MD5 };
   my $have_md5 = eval { require MD5 };
   my $data = shift;

   if ( ! $have_digest_md5 ) {
     return MD5->hexhash($data);
   } else {
     return Digest::MD5->new->add($data)->hexdigest;
   }
}

sub gather_output {
   my $cmd = shift;
   my $tmpfile = compat_tmpnam();
   local $/ = undef();
   system("$cmd > $tmpfile");
   open(my $CMDH, "<", $tmpfile);
   my $ret = <$CMDH>;
   close($CMDH);
   unlink($tmpfile);
   return $ret;
}

if ( $git_present ) {
   my @bits = split /\s+/, `git config --get-regexp "^remote.*.url\$"`;
   $gitinfo{url} = $bits[1];
   chomp $gitinfo{url};
   $gitinfo{revision} = `git rev-parse HEAD`;
   chomp $gitinfo{revision};
   $gitinfo{branch} = `git for-each-ref --format="\%(refname:short)" \$(git symbolic-ref HEAD 2>/dev/null || git show-ref -s HEAD)`;
   chomp $gitinfo{branch};
   @bits = split /\s+/, `git describe --tags --exact-match HEAD 2>/dev/null`;
   $bits[0] = "" unless exists $bits[0];
   $gitinfo{tag} = $bits[0];
   $gitinfo{branch} = $gitinfo{tag} if ($gitinfo{tag} =~ m@.@);
} else {
   $gitinfo{url} = "http://nowhere/tarball/";
   $gitinfo{revision} = "unknown";
   $gitinfo{branch} = "tarball";
   $gitinfo{tag} = "";
}

my %gitstatus; # The Git status output

if ( $git_present ) {
   foreach my $line (split(/\n/, gather_output("git status --porcelain"))) {
      chomp $line;
      my ($X, $Y, $fp) = ($line =~ /^(.)(.) (.+)$/);
      my $fn = $fp;
      $fn = ($fp =~ /(.+) ->/) if ($fp =~ / -> /);
      next unless (care_about_file($fn));
      # Normalise $X and $Y (WT and index) into a simple A/M/D etc
      
      $gitstatus{$fn} = "$X$Y";
   }
}

my %userinfo; # The information about the current user

{
   my @pwent = getpwuid($<);
   $userinfo{USERNAME} = $pwent[0];
   my $gecos = $pwent[6];
   $gecos =~ s/,.+//g;
   $gecos =~ s/"/'/g;
   $gecos =~ s/\\/\\\\/g;
   $userinfo{GECOS} = $gecos;
}

# The current date, in AmigaOS version friendly format (dd.mm.yyyy)

my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime();
my $compiledate = sprintf("%02d.%02d.%d",$mday,$mon+1,$year+1900);
chomp $compiledate;

# Spew the testament out

my $testament = "";

$testament .= "#define USERNAME \"$userinfo{USERNAME}\"\n";
$testament .= "#define GECOS \"$userinfo{GECOS}\"\n";

my $qroot = $root;
$qroot =~ s/"/\\"/g;

my $hostname = $ENV{HOSTNAME};

unless ( defined($hostname) && $hostname ne "") {
   # Try hostname command if env-var empty
   $hostname = gather_output("hostname");
   chomp $hostname;
}

$hostname = "unknown-host" unless (defined($hostname) && $hostname ne "");
$hostname =~ s/"/\\"/g;

$testament .= "#define WT_ROOT \"$qroot\"\n";
$testament .= "#define WT_HOSTNAME \"$hostname\"\n";
$testament .= "#define WT_COMPILEDATE \"$compiledate\"\n";

my $cibuild = $ENV{CI_BUILD};
if (defined ($cibuild) && ($cibuild ne '')) {
   $testament .= "#define CI_BUILD \"$cibuild\"\n";
}

$testament .= "#define WT_BRANCHPATH \"$gitinfo{branch}\"\n";

if ($gitinfo{branch} =~ m@^master$@) {
   $testament .= "#define WT_BRANCHISMASTER 1\n";
}
if ($gitinfo{tag} =~ m@.@) {
   $testament .= "#define WT_BRANCHISTAG 1\n";
   $testament .= "#define WT_TAGIS \"$gitinfo{tag}\"\n";
}
if ($gitinfo{url} =~ m@/tarball/@) {
   $testament .= "#define WT_NO_GIT 1\n";
}
$testament .= "#define WT_REVID \"$gitinfo{revision}\"\n";
$testament .= "#define WT_MODIFIED " . scalar(keys %gitstatus) . "\n";
$testament .= "#define WT_MODIFICATIONS {\\\n";
my $doneone = 0;
foreach my $filename (sort keys %gitstatus) {
   if ($doneone) {
      $testament .= ", \\\n";
   }
   $testament .= "  { \"$filename\", \"$gitstatus{$filename}\" }";
   $doneone = 1;
}
$testament .= " \\\n}\n";

my $oldcsum = "";
if ( -e $targetfile ) {
   open(my $OLDVALUES, "<", $targetfile);
   foreach my $line (readline($OLDVALUES)) {
      if ($line =~ /MD5:([0-9a-f]+)/) {
         $oldcsum = $1;
      }
   }
   close($OLDVALUES);
}

my $newcsum = compat_md5_hex($testament);

if ($oldcsum ne $newcsum) {
   print "TESTMENT: $targetfile\n";
   open(my $NEWVALUES, ">", $targetfile) or die "$!";
   print $NEWVALUES "/* ", $targetfile,"\n";
   print $NEWVALUES <<'EOS';
 * 
 * Revision testament.
 *
 * *WARNING* this file is automatically generated by git-testament.pl 
 *
 * Copyright 2012 Greyhound Browser Project
 */

EOS

   print $NEWVALUES "#ifndef GREYHOUND_REVISION_TESTAMENT\n";
   print $NEWVALUES "#define GREYHOUND_REVISION_TESTAMENT \"$newcsum\"\n\n";
   print $NEWVALUES "/* Revision testament checksum:\n";
   print $NEWVALUES " * MD5:", $newcsum,"\n */\n\n";
   print $NEWVALUES "/* Revision testament: */\n";
   print $NEWVALUES $testament;
   print $NEWVALUES "\n#endif\n";
   close($NEWVALUES);
     foreach my $unwanted (@ARGV) {
        next unless(-e $unwanted);
        print "TESTAMENT: Removing $unwanted\n";
        system("rm", "-f", "--", $unwanted);
     }
} else {
   print "TESTMENT: unchanged\n";
}

exit 0;

sub care_about_file {
   my ($fn) = @_;
   return 0 if ($fn =~ /\.d$/); # Don't care for extraneous DEP files
   return 0 if ($fn =~ /\.a$/); # Don't care for extraneous archive files
   return 0 if ($fn =~ /\.md5$/); # Don't care for md5sum files
   return 0 if ($fn =~ /\.map$/); # Don't care for map files
   return 0 if ($fn =~ /\.gitt$/); # Don't care for testament temp files
   return 1;
}
