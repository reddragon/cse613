for n in 16 32 1024
do
  echo "Doing it for $n"
  cilkview -trials all ./a.out 1 "samples/rand-$n-in.txt"
done
