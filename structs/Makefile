CC = gcc-14
CFLAGS = -Wall -I. -g -O3

OUTPUT = test
SOURCES = list.c map.c set.c

TEST_SRCS := $(wildcard tests/test_*.c)
TEST_BINS := $(patsubst tests/%.c,tests/%.out,$(TEST_SRCS))

all: $(OUTPUT)

force_build:
	@if [ -e $(OUTPUT) ]; then rm -f $(OUTPUT); fi

$(OUTPUT): $(SOURCES)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SOURCES)

tests/%.out: tests/%.c $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^

run-test: $(TEST_BINS)
	@echo "Running tests..."
	@for t in $(TEST_BINS); do \
		echo ">> $$t"; \
		"./$$t"; \
		rm -f "$$t"; \
		rm -rf "$$t.dSYM"; \
	done
	@echo "All tests completed and binaries removed."

clean:
	rm -f $(OUTPUT) $(TEST_BINS)

.PHONY: all clean force_build run-test
