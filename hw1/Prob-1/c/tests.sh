for n in 16 32 1024
do
  echo "Doing it for $n"
  cilkview -verbose ./a.out $n < "samples/rand-$n-in.txt" > cout.out
done
