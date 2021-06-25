target: unified.c


gen: gen_unified.c
	gcc -o gen gen_unified.c

unified.c: UnifiedList.txt gen
	gen  UnifiedList.txt

