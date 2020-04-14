build: producer consumer

producer: producer.c Utilities.h
	gcc producer.c -o producer.out

consumer: consumer.c Utilities.h
	gcc consumer.c -o consumer.out

clean:
	rm *.out