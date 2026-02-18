include ../module.mk

# --- Submission ---
submission.zip: $(wildcard *.c) $(wildcard *.h)
	@echo "=== Creating submission.zip ==="
	@zip -j $@ *.c *.h
	@echo "=== Upload submission.zip to Gradescope ==="

clean: clean-submission
clean-submission:
	@rm -f submission.zip

.PHONY: clean-submission
