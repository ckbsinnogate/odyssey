
/*
 * machinarium.
 *
 * Cooperative multitasking engine.
*/

#include <machinarium.h>
#include <machinarium_test.h>

static machine_channel_t channel;

static void
test_coroutine2(void *arg)
{
	machine_msg_t msg;
	msg = machine_channel_read(channel, UINT32_MAX);
	test(msg == NULL);
	test(machine_cancelled());
}

static void
test_coroutine(void *arg)
{
	channel = machine_channel_create();
	test(channel != NULL);

	int id;
	id = machine_coroutine_create(test_coroutine2, NULL);
	machine_sleep(0);
	machine_cancel(id);
	machine_join(id);

	machine_channel_free(channel);
}

void
test_channel_cancel(void)
{
	machinarium_init();

	int id;
	id = machine_create("test", test_coroutine, NULL);
	test(id != -1);

	int rc;
	rc = machine_wait(id);
	test(rc != -1);

	machinarium_free();
}
