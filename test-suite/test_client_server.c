
/*
 * machinarium.
 *
 * Cooperative multitasking engine.
*/

#include <machinarium.h>
#include <machinarium_test.h>

#include <string.h>
#include <arpa/inet.h>

static void
server(void *arg)
{
	machine_t machine = arg;
	machine_io_t server = machine_create_io(machine);
	test(server != NULL);

	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr("127.0.0.1");
	sa.sin_port = htons(7778);
	int rc;
	rc = machine_bind(server, (struct sockaddr*)&sa);
	test(rc == 0);

	machine_io_t client;
	rc = machine_accept(server, &client, 16, INT_MAX);
	test(rc == 0);

	char msg[] = "hello world";
	rc = machine_write(client, msg, sizeof(msg), INT_MAX);
	test(rc == 0);

	rc = machine_close(client);
	test(rc == 0);
	machine_free_io(client);

	rc = machine_close(server);
	test(rc == 0);
	machine_free_io(server);
}

static void
client(void *arg)
{
	machine_t machine = arg;
	machine_io_t client = machine_create_io(machine);
	test(client != NULL);

	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr("127.0.0.1");
	sa.sin_port = htons(7778);
	int rc;
	rc = machine_connect(client, (struct sockaddr*)&sa, INT_MAX);
	test(rc == 0);

	char buf[16];
	rc = machine_read(client, buf, 12, INT_MAX);
	test(rc == 0);
	test(memcmp(buf, "hello world", 12) == 0);

	rc = machine_read(client, buf, 1, INT_MAX);
	/* eof */
	test(rc == -1);

	rc = machine_close(client);
	test(rc == 0);
	machine_free_io(client);

	machine_stop(machine);
}

void
test_client_server(void)
{
	machine_t machine = machine_create();
	test(machine != NULL);

	int rc;
	rc = machine_create_fiber(machine, server, machine);
	test(rc != -1);

	rc = machine_create_fiber(machine, client, machine);
	test(rc != -1);

	machine_start(machine);

	rc = machine_free(machine);
	test(rc != -1);
}
