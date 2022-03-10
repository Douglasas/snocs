
for i in {0..15}
do

  printf "results/0/ext_${i}_out results/1/ext_${i}_out: "
  diff -y --suppress-common-lines results/0/ext_${i}_out results/1/ext_${i}_out | wc -l
  #
  # mkdir results/$i
  #
  # mv $MY_WORKSPACE/*out results/$i/
  # mv $MY_WORKSPACE/Results/ results/$i/Results/

done
