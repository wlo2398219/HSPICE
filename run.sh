read -p "upper bound: " Q
for ((x=50; x<=$Q;x+=10))
do
./a.out $x > mesh.sp
./main mesh.sp $x 101
done
