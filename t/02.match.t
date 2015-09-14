
sub match()  { \'match' }
sub nomatch() { \'nomatch'}

sub regex($$$) 
{
	my ($pat,  $match, $str) = @_;

	

	return ( './run', ['-m', $pat, 'echo', $str], 
	sub
	{
		return shift->{result} == 0 if ($match == match); 
		return shift->{result} == 5 if ($match == nomatch); 
 
	} );
}

name 'flavor_ere';
test regex 'a+b', match, 'aaabcccc';

name 'flavor_not_bre';
test regex 'a\{1,\}b', nomatch, 'aaabcccc';

name 'backslash_w';
test regex '\w\W\w\W', match, 'a!1!';

name 'backslash_d';
test regex '\d\D\d\D\d\D', match, '1 2 3 ';

name 'backslash_s';
test regex '\S\s\S\s', match,  ". .\t";

name 'count_fixed';
test regex '_a{4,4}_', match, '_aaaa_';

name 'count_min';
test regex '_a{4,}_', match, '_aaaaaaaa_';
 
name 'count_max';
test regex '_a{1,3}_', match, '_aa_';

name 'backref';
test regex '(\w+) = \1', match, 'QWERTY_123 = QWERTY_123'; 

name 'no_backref';
test regex '(\w+) = \1', nomatch, 'QWERTY_123 = QUARTZ_123'; 
