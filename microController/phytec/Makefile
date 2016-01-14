APPLICATION = periph_adc
BOARD ?= native
RIOTBASE ?= $(CURDIR)/../../..
#include ../../../tests/Makefile.tests_common
#DISABLE_MODULE += auto_init

FEATURES_REQUIRED = periph_adc
FEATURES_REQUIRED = periph_i2c
FEATURES_REQUIRED = periph_pwm

USEMODULE += xtimer
USEMODULE += mpl3115a2
USEMODULE += hdc1000
USEMODULE += servo

USEMODULE += gnrc_netif_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_udp
USEMODULE += gnrc_conn_udp
USEMODULE += posix_sockets


LINKFLAGS += -u _printf_float

ifneq (,$(TEST_MPL3115A2_I2C))
  CFLAGS += -DTEST_MPL3115A2_I2C=$(TEST_MPL3115A2_I2C)
else
  CFLAGS += -DTEST_MPL3115A2_I2C=I2C_0
endif
ifneq (,$(TEST_MPL3115A2_ADDR))
  CFLAGS += -DTEST_MPL3115A2_ADDR=$(TEST_MPL3115A2_ADDR)
else
  CFLAGS += -DTEST_MPL3115A2_ADDR=0x60
endif

ifneq (,$(TEST_HDC1000_I2C))
  CFLAGS += -DTEST_HDC1000_I2C=$(TEST_HDC1000_I2C)
else
  # set random default
  CFLAGS += -DTEST_HDC1000_I2C=I2C_0
endif
ifneq (,$(TEST_HDC1000_ADDR))
  CFLAGS += -DTEST_HDC1000_ADDR=$(TEST_HDC1000_ADDR)
else
  # set random default
  CFLAGS += -DTEST_HDC1000_ADDR=0x43
endif

include $(RIOTBASE)/Makefile.include

