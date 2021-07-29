src := *.c
include := *.h
bin := web
opt := -w -O2 -lpthread
cc := gcc

run: $(bin)
	@./$(bin)

$(bin): $(src) $(include)
	@$(cc) $(src) -o $(bin) $(opt)
