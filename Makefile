cc=gcc

files = memallocator-nsf-demo-vipzone/memallocator-nsf-demo-vipzone memallocator-nsf-demo-vanilla/memallocator-nsf-demo-vanilla
      
all : $(files)

% : %.c
	$(cc) $< -o $@

clean :
	rm -f $(files)
