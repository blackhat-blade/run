
my $dot = './run';

name 'true_is_true';
test $dot, ['true'];

name 'false_is_false';
test $dot, ['false'], sub { shift->{result} != 0 };

name 'nocmd';
test $dot, [], sub { shift->{result} != 0 };

name 'basic_match';
test $dot, ['-m', 'testABC', 'echo', 'testABC'];

name 'match_not';
test $dot, ['-m', 'testABC', 'echo', 'testABX'], sub { shift->{result} != 0 };

name 'file_not_found';
test $dot, ['/var/empty/shouldnotexist/evenless'], sub { shift->{result} != 0 };

name 'isdir';
test $dot, ['/tmp'], sub { shift->{result} != 0 };

name 'isdevice';
test $dot, ['/dev/null'], sub { shift->{result} != 0};

name 'recursive';
test $dot, ['./run', 'true'];

name 'recursive_not';
test $dot, ['./run', 'false'], sub { shift->{result} != 0 };
