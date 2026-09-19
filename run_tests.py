#!/usr/bin/env python3
"""Compile and verify Lab 1 in a temporary directory; originals stay unchanged."""
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

PROGRAMS = ['file_info', 'file_reader', 'mycopy', 'standard_io',
            'mycopy_stdio', 'fd_test', 'myhead']
base = Path(__file__).resolve().parent
checks = 0

def check(condition, name):
    global checks
    if not condition:
        raise AssertionError(name)
    checks += 1
    print('PASS:', name)

with tempfile.TemporaryDirectory(prefix='unix_lab1_') as name:
    work = Path(name)
    work.chmod(0o755)
    def run(args, **kwargs):
        return subprocess.run(args, cwd=work, capture_output=True, **kwargs)
    for prog in PROGRAMS:
        r = run(['gcc', '-std=c11', '-Wall', '-Wextra', '-Wpedantic', '-Werror',
                 '-o', prog, str(base / (prog + '.c'))])
        check(r.returncode == 0, prog + ' compiles without warnings')
    sample = (base/'input.txt').read_bytes()
    (work/'input.txt').write_bytes(sample)
    r = run(['./file_info'])
    check(r.returncode == 0 and r.stdout == b'File descriptor: 3\n', 'file_info descriptor 3')
    r = run(['./file_reader'])
    check(r.returncode == 0 and r.stdout == sample and
          r.stderr == b'Read 24 bytes\nRead 24 bytes\nRead 3 bytes\nRead 0 bytes\n',
          'reader preserves bytes and reports reads including EOF')
    for prog in ['mycopy', 'mycopy_stdio']:
        for label, data in [('text', sample), ('empty', b''),
                            ('binary', bytes(range(256))*100),
                            ('large', b'X'*100000+b'\nlast\x00')]:
            (work/'source').write_bytes(data)
            (work/'destination').write_bytes(b'old trailing bytes'*100)
            r = run(['./'+prog, 'source', 'destination'])
            check(r.returncode == 0 and (work/'destination').read_bytes() == data,
                  prog+' '+label+' copy and truncation')
        for args in [[], ['a','b','c'], ['missing.txt','out'], ['input.txt','.']]:
            r = run(['./'+prog]+args)
            check(r.returncode != 0 and bool(r.stderr), prog+' rejects '+repr(args))
        r = run(['./'+prog,'input.txt','input.txt'])
        check(r.returncode != 0 and (work/'input.txt').read_bytes() == sample,
              prog+' rejects same file without data loss')
        alias = work/'alias'
        os.link(work/'input.txt',alias)
        r = run(['./'+prog,'input.txt','alias'])
        check(r.returncode != 0 and alias.read_bytes() == sample, prog+' rejects hard-link alias')
        alias.unlink()
        blocked = work/'blocked'
        blocked.mkdir(exist_ok=True)
        blocked.chmod(0o555)
        (work/'input.txt').chmod(0o644)
        # Run without DAC-bypass privileges; an unrestricted root shell is unsuitable.
        r = run(['./'+prog,'input.txt','blocked/output.txt'])
        check(r.returncode != 0 and b'Permission denied' in r.stderr,
              prog+' destination permission denial')
        blocked.chmod(0o755)
        r = run(['./'+prog,'input.txt','/dev/full'])
        check(r.returncode != 0 and bool(r.stderr), prog+' detects output device full')
    r = run(['./standard_io'])
    check(r.returncode == 0 and r.stdout == b'This is standard output.\n' and
          r.stderr == b'This is standard error.\n', 'stdout and stderr separation')
    r = run(['./fd_test'])
    check(r.returncode == 0 and r.stdout ==
          b'stdin=0 stdout=1 stderr=2\nfd_a.txt: 3\nfd_b.txt: 4\nfd_c.txt: 5\nClosed descriptor: 4\nfd_d.txt: 4\n',
          'descriptor 4 reused experimentally')
    data = b''.join(('Line %d\n'%i).encode() for i in range(1,16))
    for label, contents, count in [('default',data,10),('five',data,5),
        ('zero',data,0),('beyond EOF',data,100),('empty',b'',10),
        ('no final newline',b'one\ntwo',10),('long line',b'A'*9000+b'\nend',1),
        ('buffer boundary',b'A'*4095+b'\nnext\n',1)]:
        (work/'lines.txt').write_bytes(contents)
        args = ['./myhead','lines.txt'] if label == 'default' else ['./myhead',str(count),'lines.txt']
        r = run(args)
        expected = run(['head','-n',str(count),'lines.txt'])
        check(r.returncode == 0 and r.stdout == expected.stdout, 'myhead '+label+' matches UNIX head')
    for args in [[], ['-1','input.txt'],['abc','input.txt'],['1x','input.txt'],
                 ['999999999999999999999999999999','input.txt'],['missing.txt']]:
        r=run(['./myhead']+args)
        check(r.returncode != 0 and bool(r.stderr), 'myhead rejects '+repr(args))
    (work/'input.txt').unlink()
    for prog in ['file_info','file_reader']:
        r=run(['./'+prog])
        check(r.returncode != 0 and b'No such file or directory' in r.stderr,
              prog+' missing input error')
    r = run(['gcc','-Wall','-Wextra','-Werror','-DFORCE_OPEN_ERROR','-o',
             'file_reader_error',str(base/'file_reader.c')])
    check(r.returncode == 0, 'intentional-error reader variant compiles')
    r = run(['./file_reader_error'])
    check(r.returncode != 0 and r.stdout == b'' and b'intentionally_missing.txt' in r.stderr,
          'intentional error is only on stderr')
    r = run(['gcc','-Wall','-Wextra','-Werror','-DUSE_LITERAL_DESCRIPTORS','-o',
             'standard_io_literal',str(base/'standard_io.c')])
    check(r.returncode == 0, 'literal descriptor variant compiles')
    r = run(['./standard_io_literal'])
    check(r.returncode == 0 and r.stdout == b'This is standard output.\n' and
          r.stderr == b'This is standard error.\n', 'literal descriptor behavior matches constants')
print('\n%d checks passed.'%checks)
