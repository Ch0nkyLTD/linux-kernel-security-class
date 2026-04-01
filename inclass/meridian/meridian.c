/*
 * meridian.c — MERIDIAN DEFENSE GROUP Secure Terminal
 *
 * Vulnerable TCP service for the kernel security capstone.
 * Runs as unprivileged user "analyst" on port 1337.
 *
 * The vulnerability: the "submit" command mmap()s an RWX page,
 * reads raw bytes from the socket into it, and clone()s a thread
 * that executes the data as code.  Students exploit this to get
 * code execution, then pivot to a vulnerable kernel driver for LPE.
 *
 * FOR EDUCATIONAL USE ONLY — this server is deliberately vulnerable.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sched.h>
#include <ctype.h>
#include <time.h>
#include <pwd.h>

#define PORT            1337
#define MAX_LINE        256
#define MAX_SUBMIT      (1024 * 1024)   /* 1 MB */
#define REPORTS_DIR     "/usr/local/share/meridian/reports"
#define MAX_CLIENTS     8
#define STACK_SIZE      (64 * 1024)

static volatile int running = 1;
static time_t       start_time;
static int          client_count;

/* ── Report database ──────────────────────────────────────────────────────── */

struct report {
	const char *id;
	const char *title;
	const char *filename;
};

static const struct report reports[] = {
	{ "MDG-2024-0117", "SIGINT Summary — Eastern Mediterranean",   "sigint_summary.txt"   },
	{ "MDG-2024-0204", "OSINT Digest — Central Asian Infrastructure", "osint_digest.txt"  },
	{ "MDG-2024-0315", "HUMINT Debrief — Source CARDINAL",         "humint_debrief.txt"   },
	{ "MDG-2024-0402", "GEOINT Analysis — Coastal Installations",  "geoint_analysis.txt"  },
	{ "MDG-2024-0419", "Threat Assessment — APT Activity Q1",      "threat_assess.txt"    },
	{ "MDG-2024-0501", "Cyber Incident Report — Perimeter Probe",  "cyber_incident.txt"   },
};
#define NUM_REPORTS (sizeof(reports) / sizeof(reports[0]))

/* ── Helpers ──────────────────────────────────────────────────────────────── */

static void send_str(int fd, const char *s)
{
	size_t len = strlen(s);
	ssize_t n;
	while (len > 0) {
		n = write(fd, s, len);
		if (n <= 0) return;
		s   += n;
		len -= n;
	}
}

static void send_banner(int fd)
{
	send_str(fd,
		"\r\n"
		"  ==========================================\r\n"
		"   MERIDIAN DEFENSE GROUP\r\n"
		"   Secure Terminal v3.2 — UNCLASSIFIED\r\n"
		"  ==========================================\r\n"
		"  Welcome, Analyst.\r\n"
		"  Type 'help' for available commands.\r\n"
		"\r\n");
}

static void send_prompt(int fd)
{
	send_str(fd, "  analyst> ");
}

/* Read one line from socket, stripping \r\n.  Returns length or -1. */
static int read_line(int fd, char *buf, int maxlen)
{
	int i = 0;
	char c;
	ssize_t n;
	while (i < maxlen - 1) {
		n = read(fd, &c, 1);
		if (n <= 0) return -1;
		if (c == '\n') break;
		if (c != '\r')
			buf[i++] = c;
	}
	buf[i] = '\0';
	return i;
}

/* ── Command handlers ─────────────────────────────────────────────────────── */

static void cmd_help(int fd)
{
	send_str(fd,
		"  Available commands:\r\n"
		"    reports           - List available intelligence reports\r\n"
		"    read <id>         - Read a report (UNCLASSIFIED only)\r\n"
		"    submit <size>     - Submit field data (raw bytes)\r\n"
		"    search <keyword>  - Search report database\r\n"
		"    status            - System status\r\n"
		"    whoami            - Current clearance level\r\n"
		"    quit              - Disconnect\r\n"
		"\r\n");
}

static void cmd_reports(int fd)
{
	char line[256];
	send_str(fd, "  Classification: UNCLASSIFIED\r\n\r\n");
	send_str(fd, "  ID               Title\r\n");
	send_str(fd, "  ──────────────── ──────────────────────────────────────────\r\n");
	for (size_t i = 0; i < NUM_REPORTS; i++) {
		snprintf(line, sizeof(line), "  %-18s %s\r\n",
			 reports[i].id, reports[i].title);
		send_str(fd, line);
	}
	send_str(fd, "\r\n");
}

static void cmd_read(int fd, const char *arg)
{
	char path[512];
	char buf[512];
	FILE *fp;

	if (!arg || !*arg) {
		send_str(fd, "  Usage: read <report-id>\r\n");
		return;
	}

	/* Find the report */
	const struct report *rpt = NULL;
	for (size_t i = 0; i < NUM_REPORTS; i++) {
		if (strcasecmp(arg, reports[i].id) == 0) {
			rpt = &reports[i];
			break;
		}
	}
	if (!rpt) {
		send_str(fd, "  ERROR: Report not found. Use 'reports' to list.\r\n");
		return;
	}

	snprintf(path, sizeof(path), "%s/%s", REPORTS_DIR, rpt->filename);
	fp = fopen(path, "r");
	if (!fp) {
		send_str(fd, "  ERROR: Report file unavailable.\r\n");
		return;
	}

	snprintf(buf, sizeof(buf),
		 "\r\n  ── %s ─────────────────────────────\r\n"
		 "  %s\r\n"
		 "  Classification: UNCLASSIFIED\r\n\r\n",
		 rpt->id, rpt->title);
	send_str(fd, buf);

	while (fgets(buf, sizeof(buf), fp)) {
		/* Prefix each line for terminal formatting */
		send_str(fd, "  ");
		send_str(fd, buf);
		/* Ensure \r\n line endings */
		size_t len = strlen(buf);
		if (len > 0 && buf[len - 1] == '\n' && (len < 2 || buf[len - 2] != '\r'))
			send_str(fd, "\r");
	}
	fclose(fp);
	send_str(fd, "\r\n");
}

static void cmd_search(int fd, const char *keyword)
{
	char line[256];
	int found = 0;

	if (!keyword || !*keyword) {
		send_str(fd, "  Usage: search <keyword>\r\n");
		return;
	}

	send_str(fd, "  Search results:\r\n");
	for (size_t i = 0; i < NUM_REPORTS; i++) {
		if (strcasestr(reports[i].title, keyword)) {
			snprintf(line, sizeof(line), "    %s  %s\r\n",
				 reports[i].id, reports[i].title);
			send_str(fd, line);
			found++;
		}
	}
	if (!found)
		send_str(fd, "    (no matching reports)\r\n");
	send_str(fd, "\r\n");
}

static void cmd_status(int fd)
{
	char buf[256];
	time_t now = time(NULL);
	long uptime = (long)(now - start_time);

	snprintf(buf, sizeof(buf),
		 "  System Status\r\n"
		 "  ─────────────\r\n"
		 "  Uptime:     %ld:%02ld:%02ld\r\n"
		 "  PID:        %d\r\n"
		 "  User:       analyst\r\n"
		 "  Clients:    %d / %d\r\n"
		 "\r\n",
		 uptime / 3600, (uptime % 3600) / 60, uptime % 60,
		 getpid(), client_count, MAX_CLIENTS);
	send_str(fd, buf);
}

static void cmd_whoami(int fd)
{
	send_str(fd,
		"  User:       analyst\r\n"
		"  UID:        1001\r\n"
		"  Clearance:  UNCLASSIFIED\r\n"
		"  Note:       Director-level access (TS/SCI) required for\r\n"
		"              classified reports in /home/director/classified/\r\n"
		"\r\n");
}

/* ── The vulnerability: submit command ────────────────────────────────────── */

struct submit_args {
	void *region;
	int   client_fd;
};

static int submit_thread(void *arg)
{
	struct submit_args *sa = (struct submit_args *)arg;
	void (*entry)(void) = (void (*)(void))sa->region;

	/* Execute the submitted data as code.
	 * The thread inherits the client socket fd via CLONE_VM. */
	entry();
	return 0;
}

static void cmd_submit(int fd, const char *arg)
{
	unsigned long size;
	void *region;
	char *stack;
	char buf[128];
	ssize_t total, n;
	struct submit_args sa;

	if (!arg || !*arg) {
		send_str(fd, "  Usage: submit <size>\r\n");
		return;
	}

	size = strtoul(arg, NULL, 10);
	if (size == 0 || size > MAX_SUBMIT) {
		snprintf(buf, sizeof(buf),
			 "  ERROR: Size must be 1-%d bytes.\r\n", MAX_SUBMIT);
		send_str(fd, buf);
		return;
	}

	/* Step 1: Allocate RWX memory */
	region = mmap(NULL, size,
		      PROT_READ | PROT_WRITE | PROT_EXEC,
		      MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (region == MAP_FAILED) {
		send_str(fd, "  ERROR: Memory allocation failed.\r\n");
		return;
	}

	snprintf(buf, sizeof(buf),
		 "  Receiving %lu bytes of field data...\r\n", size);
	send_str(fd, buf);

	/* Step 2: Read raw bytes from socket into RWX region */
	total = 0;
	while ((size_t)total < size) {
		n = read(fd, (char *)region + total, size - total);
		if (n <= 0) {
			munmap(region, size);
			return;
		}
		total += n;
	}

	send_str(fd, "  Processing submission in background...\r\n");

	/* Step 3: Allocate stack for clone */
	stack = mmap(NULL, STACK_SIZE,
		     PROT_READ | PROT_WRITE,
		     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (stack == MAP_FAILED) {
		munmap(region, size);
		send_str(fd, "  ERROR: Stack allocation failed.\r\n");
		return;
	}

	/* Step 4: clone() with CLONE_VM — new thread shares address space
	 * and inherits the client socket fd */
	sa.region    = region;
	sa.client_fd = fd;

	if (clone(submit_thread, stack + STACK_SIZE,
		  CLONE_VM | SIGCHLD, &sa) == -1) {
		munmap(region, size);
		munmap(stack, STACK_SIZE);
		send_str(fd, "  ERROR: Thread creation failed.\r\n");
		return;
	}

	/* Note: we intentionally do NOT free the RWX region or stack here.
	 * The child thread is still executing in it.  In a real server this
	 * would be a resource leak — but this is intentionally vulnerable. */
}

/* ── Client handler ───────────────────────────────────────────────────────── */

static void handle_client(int fd)
{
	char line[MAX_LINE];
	char *cmd, *arg;

	client_count++;
	send_banner(fd);

	while (running) {
		send_prompt(fd);
		if (read_line(fd, line, sizeof(line)) < 0)
			break;

		/* Skip empty lines */
		cmd = line;
		while (*cmd == ' ' || *cmd == '\t') cmd++;
		if (!*cmd) continue;

		/* Split command and argument */
		arg = cmd;
		while (*arg && *arg != ' ' && *arg != '\t') arg++;
		if (*arg) {
			*arg++ = '\0';
			while (*arg == ' ' || *arg == '\t') arg++;
		}

		/* Dispatch */
		if (strcasecmp(cmd, "help") == 0) {
			cmd_help(fd);
		} else if (strcasecmp(cmd, "reports") == 0) {
			cmd_reports(fd);
		} else if (strcasecmp(cmd, "read") == 0) {
			cmd_read(fd, arg);
		} else if (strcasecmp(cmd, "submit") == 0) {
			cmd_submit(fd, arg);
		} else if (strcasecmp(cmd, "search") == 0) {
			cmd_search(fd, arg);
		} else if (strcasecmp(cmd, "status") == 0) {
			cmd_status(fd);
		} else if (strcasecmp(cmd, "whoami") == 0) {
			cmd_whoami(fd);
		} else if (strcasecmp(cmd, "quit") == 0 ||
			   strcasecmp(cmd, "exit") == 0) {
			send_str(fd, "  Session terminated.\r\n");
			break;
		} else {
			send_str(fd, "  Unknown command. Type 'help'.\r\n");
		}
	}

	client_count--;
	close(fd);
}

/* ── Signal handler ───────────────────────────────────────────────────────── */

static void sighandler(int sig)
{
	(void)sig;
	running = 0;
}

/* ── Main ─────────────────────────────────────────────────────────────────── */

int main(void)
{
	int server_fd, client_fd;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	int opt = 1;
	pid_t pid;

	signal(SIGINT, sighandler);
	signal(SIGTERM, sighandler);
	signal(SIGCHLD, SIG_IGN);   /* auto-reap children */

	start_time = time(NULL);

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		perror("socket");
		return 1;
	}

	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port        = htons(PORT);

	if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		close(server_fd);
		return 1;
	}

	if (listen(server_fd, MAX_CLIENTS) < 0) {
		perror("listen");
		close(server_fd);
		return 1;
	}

	fprintf(stderr, "meridian: listening on port %d\n", PORT);

	while (running) {
		client_fd = accept(server_fd, (struct sockaddr *)&addr, &addrlen);
		if (client_fd < 0) {
			if (errno == EINTR) continue;
			perror("accept");
			continue;
		}

		if (client_count >= MAX_CLIENTS) {
			send_str(client_fd,
				 "  Maximum sessions reached. Try again later.\r\n");
			close(client_fd);
			continue;
		}

		pid = fork();
		if (pid < 0) {
			perror("fork");
			close(client_fd);
		} else if (pid == 0) {
			/* Child — handle client */
			close(server_fd);
			handle_client(client_fd);
			_exit(0);
		} else {
			/* Parent — keep accepting */
			close(client_fd);
		}
	}

	close(server_fd);
	fprintf(stderr, "meridian: shutting down\n");
	return 0;
}
