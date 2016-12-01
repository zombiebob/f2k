/*
  Copyright (C) 2016 Eneo Tecnologia S.L.
  Author: Eugenio Perez <eupm90@gmail.com>
  Based on Luca Deri nprobe 6.22 collector

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Affero General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "rb_netflow_test.h"

#include "f2k.h"

#include <setjmp.h>
#include <cmocka.h>

static const NetFlow5Record record1 = {
	.flowHeader = {
		.version = 0x0500,     /* Current version=5*/
		.count = 0x0300,       /* The number of records in PDU. */
		.sys_uptime = 12345,    /* Current time in msecs since router booted */
		.unix_secs = 12345,    /* Current seconds since 0000 UTC 1970 */
		.unix_nsecs = 12345,   /* Residual nanoseconds since 0000 UTC 1970 */
		.flow_sequence = 1050, /* Sequence number of total flows seen */
		.engine_type = 0,      /* Type of flow switching engine (RP,VIP,etc.)*/
		.engine_id  = 0,       /* Slot number of the flow switching engine */
		.sampleRate = 0,       /* Packet capture sample rate */
	},
	.flowRecord = {
		[0] = {
			.srcaddr = 0x08080808L,    /* Source IP Address */
			.dstaddr = 0x0A0A0A0AL,    /* Destination IP Address */
			.nexthop = 0x00000000L,    /* Next hop router's IP Address */
			.input   = 0,              /* Input interface index */
			.output  = 255,            /* Output interface index */
			.dPkts   = 0x0100,              /* Packets sent in Duration (milliseconds between 1st
			                             & last packet in this flow)*/
			.dOctets = 0x4600,             /* Octets sent in Duration (milliseconds between 1st
			                             & last packet in  this flow)*/
			.first   = 0xa8484205,     /* SysUptime at start of flow */
			.last    = 0xa8484205,     /* and of last packet of the flow */
			.srcport = 0xbb01,     /* ntohs(443)  */  /* TCP/UDP source port number (.e.g, FTP, Telnet, etc.,or equivalent) */
			.dstport = 0x7527,   /* ntohs(10101)*/  /* TCP/UDP destination port number (.e.g, FTP, Telnet, etc.,or equivalent) */
			.pad1    = 0,        /* pad to word boundary */
			.tcp_flags = 0,   /* Cumulative OR of tcp flags */
			.proto   = 2,     /* IP protocol, e.g., 6=TCP, 17=UDP, etc... */
			.tos     = 0,     /* IP Type-of-Service */
			.src_as  = 0,     /* source peer/origin Autonomous System */
			.dst_as  = 0,     /* dst peer/origin Autonomous System */
			.src_mask = 0,    /* source route's mask bits */
			.dst_mask = 0,    /* destination route's mask bits */
			.pad2    = 0,       /* pad to word boundary */
		},

		[1] = {
			.srcaddr = 0x10101112L,    /* Source IP Address */
			.dstaddr = 0x0A0A1112L,    /* Destination IP Address */
			.nexthop = 0x00000000L,    /* Next hop router's IP Address */
			.input   = 0,              /* Input interface index */
			.output  = 255,            /* Output interface index */
			.dPkts   = 0x0100,              /* Packets sent in Duration (milliseconds between 1st
			                             & last packet in this flow)*/
			.dOctets = 0x4600,             /* Octets sent in Duration (milliseconds between 1st
			                             & last packet in  this flow)*/
			.first   = 0xa8484205, /* SysUptime at start of flow */
			.last    = 0xa8484205, /* and of last packet of the flow */
			.srcport = 0xbb01,     /* ntohs(443)  */  /* TCP/UDP source port number (.e.g, FTP, Telnet, etc.,or equivalent) */
			.dstport = 0x7527, /* ntohs(10101)*/  /* TCP/UDP destination port number (.e.g, FTP, Telnet, etc.,or equivalent) */
			.pad1    = 0,      /* pad to word boundary */
			.tcp_flags = 0,    /* Cumulative OR of tcp flags */
			.proto   = 2,      /* IP protocol, e.g., 6=TCP, 17=UDP, etc... */
			.tos     = 0,      /* IP Type-of-Service */
			.src_as  = 0,      /* source peer/origin Autonomous System */
			.dst_as  = 0,      /* dst peer/origin Autonomous System */
			.src_mask = 0,     /* source route's mask bits */
			.dst_mask = 0,     /* destination route's mask bits */
			.pad2    = 0,      /* pad to word boundary */
		},

		[2] = {
			.srcaddr = 0x10111112L,    /* Source IP Address */
			.dstaddr = 0x0ABA1112L,    /* Destination IP Address */
			.nexthop = 0x00000000L,    /* Next hop router's IP Address */
			.input   = 0,              /* Input interface index */
			.output  = 255,            /* Output interface index */
			.dPkts   = 0x01900000L,              /* Packets sent in Duration (milliseconds between 1st
			                             & last packet in this flow)*/
			.dOctets = 0x56000000L,             /* Octets sent in Duration (milliseconds between 1st
			                             & last packet in  this flow)*/
			.first   = 0xa8400205L,     /* SysUptime at start of flow */
			.last    = 0xa8400205L,     /* and of last packet of the flow */
			.srcport = 0xaa01,     /* ntohs(443)  */  /* TCP/UDP source port number (.e.g, FTP, Telnet, etc.,or equivalent) */
			.dstport = 0x6027,   /* ntohs(10101)*/  /* TCP/UDP destination port number (.e.g, FTP, Telnet, etc.,or equivalent) */
			.pad1    = 0,        /* pad to word boundary */
			.tcp_flags = 0,   /* Cumulative OR of tcp flags */
			.proto   = 2,        /* IP protocol, e.g., 6=TCP, 17=UDP, etc... */
			.tos     = 0,         /* IP Type-of-Service */
			.src_as  = 0,     /* source peer/origin Autonomous System */
			.dst_as  = 0,     /* dst peer/origin Autonomous System */
			.src_mask = 0,    /* source route's mask bits */
			.dst_mask = 0,    /* destination route's mask bits */
			.pad2    = 0,       /* pad to word boundary */
		}
	}
};

#define BASE_TESTS0 \
	{.key="type",.value="netflowv5"}, \
	{.key="input_snmp",.value="0"}, \
	{.key="output_snmp",.value="65280"}, \
	{.key="tos",.value="0"}, \
	{.key="tcp_flags",.value=NULL}, \
	{.key="l4_proto",.value="2"}, \
	{.key="engine_type",.value="0"}, \
	{.key="sensor_ip",.value="4.3.2.1"}

#define BASE_TESTS \
	BASE_TESTS0, \
	{.key="src_port",.value="443"}, \
	{.key="dst_port",.value="10101"}, \
	{.key="first_switched",.value="958575823"}, \
	{.key="timestamp",.value="958575823"}

static const struct checkdata_value checkdata_values1[] = {
	BASE_TESTS,
	{.key="src",.value="8.8.8.8"},
	{.key="dst",.value="10.10.10.10"},
};

static const struct checkdata_value checkdata_values2[] = {
	BASE_TESTS,
	{.key="src",.value="18.17.16.16"},
	{.key="dst",.value="18.17.10.10"},
};

static const struct checkdata_value checkdata_values3[] = {
	BASE_TESTS0,
	{.key="src",.value="18.17.17.16"},
	{.key="dst",.value="18.17.186.10"},
	{.key="pkts", .value="36865"},
	{.key="bytes", .value="86"},
	{.key="src_port", .value="426"},
	{.key="dst_port", .value="10080"},
	{.key="first_switched", .value="958571626"},
	{.key="timestamp", .value="958571626"},
};

static int prepare_test_nf_many_v5(void **state) {
	static const struct checkdata checkdata[] = {
		{.size = RD_ARRAYSIZE(checkdata_values1), .checks = checkdata_values1},
		{.size = RD_ARRAYSIZE(checkdata_values2), .checks = checkdata_values2},
		{.size = RD_ARRAYSIZE(checkdata_values3), .checks = checkdata_values3},
	};

	struct test_params test_params[] = {
		[0] = {
			.config_json_path = "./tests/0000-testFlowV5.json",
			.host_list_path = NULL,
			.netflow_src_ip = 0x04030201,
			.netflow_dst_port = 2055,
			.record = &record1,
			.record_size = sizeof(record1),
			.checkdata = checkdata,
			.checkdata_size = RD_ARRAYSIZE(checkdata),
		},
	};

	*state = prepare_tests(test_params, RD_ARRAYSIZE(test_params));
	return *state == NULL;
}

int main() {
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup(testFlow, prepare_test_nf_many_v5),
	};

	return cmocka_run_group_tests(tests, nf_test_setup, nf_test_teardown);
}
