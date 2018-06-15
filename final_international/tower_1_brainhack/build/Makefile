SRCS=brainhack.c
BRAINHACK=brainhack
FLAGS=../flags.txt
ATTACK4=YXR0YWNrNC50eHQ=
DOWNLOAD=web/html/download
WEBLOG=web/log
SCORE=score.txt
SUBMIT=submit.log

CFLAGS=-Ofast -Wl,-z,lazy

all: $(BRAINHACK) $(FLAGS)

$(BRAINHACK): $(SRCS) flag2.txt
	cc $(CFLAGS) $< -o $@
	objcopy --add-section=attack2=flag2.txt $(BRAINHACK) $(BRAINHACK)+flag
	mv $(BRAINHACK)+flag $(BRAINHACK)

$(DOWNLOAD)/$(BRAINHACK): $(BRAINHACK)
	mkdir -p $(DOWNLOAD)
	cp $< $@
	chmod 755 $@

$(FLAGS): flag1.txt flag2.txt flag3.txt flag4.txt
	grep -h SECCON $^ > $@

$(ATTACK4): flag4.py
	python $< > $@

$(SCORE):
	echo "78" `echo -n SECCON | base64 -` > $@

$(WEBLOG)/$(SCORE): $(SCORE)
	if [ -e $@ ]; then \
		sudo chattr -a $@; \
		rm -f $@; \
	fi
	mkdir -p $(WEBLOG)
	chmod +w $(WEBLOG)
	cp $^ $@
	chmod 666 $@
	sudo chattr +a $@

$(WEBLOG)/$(SUBMIT):
	if [ -e $@ ]; then \
		sudo chattr -a $@; \
		rm -f $@; \
	fi
	mkdir -p $(WEBLOG)
	chmod +w $(WEBLOG)
	echo -n > $@
	chmod 666 $@
	sudo chattr +a $@

up: $(DOWNLOAD)/$(BRAINHACK) $(ATTACK4) $(WEBLOG)/$(SCORE) $(WEBLOG)/$(SUBMIT)
	mkdir -p nginx/log
	docker-compose up -d

stop:
	docker-compose stop

down:
	docker-compose kill -s SIGINT

clean:
	rm -f $(BRAINHACK)

clobber: down
	rm -f $(BRAINHACK)
	rm -f $(DOWNLOAD)/$(BRAINHACK)
	rm -f $(ATTACK4)
	rm -f $(SCORE)
	if [ -e $(WEBLOG)/$(SCORE) ]; then \
		sudo chattr -a $(WEBLOG)/$(SCORE); \
		rm -f $(WEBLOG)/$(SCORE); \
	fi
	if [ -e $(WEBLOG)/$(SUBMIT) ]; then \
		sudo chattr -a $(WEBLOG)/$(SUBMIT); \
		rm -f $(WEBLOG)/$(SUBMIT); \
	fi
	rm -fr web/log
	rm -fr nginx/log
