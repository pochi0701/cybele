################################################################################
# cbl makefile
#						$Revision: 1.7 $
#						$Date: 2024/03/04 19:16:51 $
################################################################################
#CROSS=mipsel-linux-uclibc-
GCC=$(CROSS)gcc
GXX=$(CROSS)g++
CC=$(CROSS)gcc
AS=$(CROSS)as
AR=$(CROSS)ar
STRIP=$(CROSS)strip
LD=$(CROSS)ld
RANLIB=$(CROSS)ranlib
#CFLAGS= -g -O3 -Wall -Dlinux -pthread
CFLAGS= -g -O3 -Wall -Dlinux -D_DEBUG -pthread -std=c++11
#CFLAGS= -O1 -Wall -Dlinux -pthread
#CFLAGS= -g -Wall -Dlinux -D_DEBUG
RM=rm

# Makefile

# プログラム名とオブジェクトファイル名
program = cybele
objs = cbl_main.o   \
       cbl_param.o  \
       cbl_listen.o \
       cbl_http.o   \
       cbl_send_file.o      \
       cbl_proxy.o  \
       cbl_cgi.o    \
       cbl_tools.o  \
       cbl_script.o \
       cbl_String.o \
       ssdp.o	\
       TinyJS.o      \
       TinyJS_Functions.o    \
       TinyJS_MathFunctions.o        \
       cbl_db.o       \
       utf8tbl.o     \
       dregex.o

# 定義d済マクロの再定義
#CC = gcc
#CFLAGS = -g -Wall

# サフィックスルール適用対象の拡張子の定義
.SUFFIXES: .cpp .o

# プライマリターゲット
$(program): $(objs)
	$(GXX) $(CFLAGS) -o $(program) $^
	strip $(program)
	mv $(program) ..

# サフィックスルール
.cpp.o:
	$(GXX) $(CFLAGS) -c $<

# ファイル削除用ターゲット
.cybele: clean
clean:
	$(RM) -f $(program) $(objs)
# ターゲット 'hello' の定義
hello:
	echo "Hello, World"

# ターゲット 'install' の定義
install: ../cybele
	mkdir -p /var/www
	mkdir -p /var/www/html
	-killall cybele
	cp ../cybele /var/www
	cp ../cbl.conf.linux /var/www/cbl.conf
	cp -r ../html /var/www
	cp -r ../database /var/www
	cp -r ../system /var/www
	cp -r ../skin /var/www
	cp ../README.md /var/www
	/var/www/cybele

# ヘッダーファイルの依存関係
