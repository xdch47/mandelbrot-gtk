#!/usr/bin/perl -w

use strict;

if (!$ARGV[0]) {
	print "usage $0 map-file [ map-file … ]\n";
	exit 0;
}

my $c_file;
my @names;
my @count_colors;

$c_file .= "\n#include \"color.h\"\n\n";
$c_file .= "#define N_(x) x\n\n";
foreach my $filename (@ARGV) {
	open(FD, "<$filename") or die $!;

	my ($name) = $filename =~ /([^\/]+).map$/;
	push(@names, $name);
	my $elements;
	my $count;
	while (<FD>) {
		$_ =~ /(\d{1,3}) +(\d{1,3}) +(\d{1,3})/;
		$elements .= sprintf("%3d, %3d, %3d, ", $1, $2, $3);
		$count++;
		$elements .= "\n\t" if ($count % 10 == 0)
	}
	push(@count_colors, $count);
	$c_file .= "static const guchar ${name}_map[3 * $count] = {\n";
	$c_file .= "\t$elements";
	$c_file .= "\n};\n\n";
}

foreach my $name (@names) {
	my $count = shift @count_colors;
	$c_file .= "const guchar *clmap_${name}(const guint iter, const guint itermax)\n{\n";
	$c_file .= "\treturn (${name}_map + (iter % $count) * 3);\n}\n\n";
}
$c_file .= "#define COLORFUNCMAP_COUNT " . scalar(@names) . "\n\n";
$c_file .= "const gchar *colorfuncmap_names[COLORFUNCMAP_COUNT] = {\n";
foreach my $name (@names) {
	$c_file .= "\tN_(\"${name}\"),\n"
}
$c_file .= "};\n\n";

$c_file .= "const ColorFunc colorfuncmap[COLORFUNCMAP_COUNT] = {\n";
foreach my $name (@names) {
	$c_file .= "\tclmap_${name},\n"
}
$c_file .= "};\n\n";

$c_file .= "int getcolorfuncmap_count()\n{\n\treturn COLORFUNCMAP_COUNT;\n}\n\n";

$c_file .= "const char *getcolorfuncmap_name(int index)\n{\n";
$c_file .= "\treturn colorfuncmap_names[index % COLORFUNCMAP_COUNT];\n}\n\n";
$c_file .= "ColorFunc getcolorfuncmap(int index)\n{\n";
$c_file .= "\treturn colorfuncmap[index % COLORFUNCMAP_COUNT];\n}\n\n";

print $c_file;
