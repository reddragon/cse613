all: merge-sort drop-caches

drop-caches: drop_caches.c
	gcc -o drop_caches drop_caches.c
	sudo chown root ./drop_caches
	sudo chmod +x ./drop_caches

merge-sort:
	cilk++ -Wall -O2 merge_sort.cilk -o merge_sort

clean:
	rm merge_sort
