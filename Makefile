apriori: apriori.c
	gcc -Wall -o apriori apriori.c

run_mushroom: apriori
	./apriori input/mushroom.dat input/mushroom_thresholds output/mushroom_fre_set

.PHONY: clean

clean:
	rm *.o apriori
