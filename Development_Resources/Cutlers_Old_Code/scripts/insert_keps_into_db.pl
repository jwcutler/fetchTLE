#!/usr/bin/perl
#===============================================================================
# Code:           keps_into_db.pl
# Author(s):      James Cutler (jwcutler@umich.edu)
# Creation Date:  09 July 2012
# Dependencies:   DBI module from perl
#===============================================================================
#
# Description:
#	TBD.
#
# Possible Upgrades:
#	- Make DB info non hard coded.
#	- Check the line checksums, lines 1 and 2.
#	- Check security issue with inserts.  We should be escaping special 
#  	  characters.
#	- Update insert and table to keep a time history of the keps. 
#		- First use insert date. Maybe? or skip to below.
#		- Then use date embedded into the kep file itself.

use DBI;

# Global DB access information.  This should be in an env var or as
# an argument.  
$db_name =       "gs_alpha";
$db_host =       "localhost";
$db_user =       "alpha";
$KEPS_TABLE =    "keps";
$ENABLE_INSERT = 1;
$VERBOSE =       1;
$TRUNCATE =      0;
$INSERT =        "INSERT INTO $KEPS_TABLE \n ".
                 " (name, epoch_time, line_one, line_two) \n" .
                 "VALUES \n";


# Open up a database connection.
my $dbh = DBI->connect("DBI:mysql:$db_name;host=$db_host", "$db_user", '')
                or die "Couldn't connect to database: " . DBI->errstr;

# Clear kep table.  Note, in Todo list, there is an upgrade option here.
if ( $TRUNCATE )
{
	$sql = "TRUNCATE $KEPS_TABLE";
	$r = $dbh->prepare($sql) 
		or die "Couldn't prepare statement: " . $dbh->errstr if $ENABLE_INSERT;
	$r->execute()            
		or die "Couldn't execute statement: " . $dbh->errstr if $ENABLE_INSERT;
}


$state = 0;

while ( $input = <STDIN> )
{
	if ($state==0)
	{
		$name  = $input; 
		$state = 1;
	}
	elsif ($state == 1)
	{
		$line1 = $input; 
		$state = 2;
	}
	elsif ( $state == 2 )
	{ 
		$line2 = $input; 
		$state = 0;

		# Clean up the entries.  Remove /r/n's at end and spaces. 
		$name  =~ s/\s+$//;    
		$line1 =~ s/\s+$//;
		$line2 =~ s/\s+$//;

		$epoch_time = get_epoch( $line1 );

		# Safe quote the strings.
		$sname = $dbh->quote($name);
		$sline1 = $dbh->quote($line1);
		$sline2 = $dbh->quote($line2);
		$sepoch_time = $dbh->quote($epoch_time);

		# Check if kep already exists.
		if ( ! keps_exists_in_db( $sname, $sline1, $sline2 ) )
		{

			$sql = "$INSERT ( $sname, $sepoch_time, $sline1, $sline2)";
			print ("Inserting: $name\n") if $VERBOSE;
			print ("           $epoch_time\n") if $VERBOSE;
			print ("           $line1\n") if $VERBOSE;
			print ("           $line2\n\n") if $VERBOSE;
			if ( $ENABLE_INSERT )
			{
		 		$r = $dbh->prepare($sql) 
					or die "Couldn't prepare statement: " . $dbh->errstr;
				$r->execute() or 
					die "Couldn't execute statement: " . $dbh->errstr;
			}
		}
		else
		{
			print "No insert, already exists.\n" if $VERBOSE;
		}

   }
} # while ( $input = <STDIN> )

$dbh->disconnect();
exit;

#-------------------------------------------------------------------------------
# Checks if the 
#-------------------------------------------------------------------------------
sub keps_exists_in_db
{
	my( $n, $l1, $l2, $s );
	( $n, $l1, $l2 ) = @_;

	print "$s\n"  if $VERBOSE;
	print "$n\n"  if $VERBOSE;
	print "$l1\n" if $VERBOSE;
	print "$l2\n" if $VERBOSE;

	$s = "SELECT COUNT(*) as myc FROM keps WHERE name=$n AND line_one=$l1 AND line_two=$l2";
 	$r = $dbh->prepare($s) or die "Couldn't prepare statement: " . $dbh->errstr;
	$r->execute()            or die "Couldn't execute statement: " . $dbh->errstr;
	my ($rowscount) = $r->fetchrow_array()	;
	#$rs = $dbh->field('myc');	
	#print "$s\n";
	print "\tRow count: $rowscount\n" if $VERBOSE;
		
	return $rowscount;

}


#-------------------------------------------------------------------------------
# Extract epoch time from the TLE.
#
# 	1 AAAAAU YYLLLPPP  BBBBB.BBBBBBBB  .CCCCCCCC  DDDDD-D  EEEEE-E F GGGGZ
#
# [BBBBB.BBBBBBBB] is the Epoch Time -- 2-digit year, followed by 3-digit 
# sequential day of the year, followed by the time represented as the fractional 
# portion of one day.
# Not going to worry about the fraction of the day for the epoch time.
#-------------------------------------------------------------------------------
sub get_epoch($)
{
	my $line = shift;

	# Extract epoch year and day of the year (ignore partial day)
	my $year = substr($line, 18, 2) + 2000;
	my $day = substr($line, 20, 3);
	my $sql =  "SELECT DATE_ADD('$year-01-01 00:00:00', INTERVAL $day DAY) AS epoch";
 	$r = $dbh->prepare($sql) or die "Couldn't prepare statement: " . $dbh->errstr;
	$r->execute()            or die "Couldn't execute statement: " . $dbh->errstr;
	@row =$r->fetchrow_array();
#	$r->bind_columns( \$epoch );
#	$r->fetch();
	return $row[0];
}


