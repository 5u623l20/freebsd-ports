PORTNAME=	php74
DISTVERSION=	7.4.33
PORTREVISION?=	0
CATEGORIES?=	lang devel www
MASTER_SITES=	PHP
DISTNAME=	php-${DISTVERSION}
DIST_SUBDIR=	php

MAINTAINER=	tz@FreeBSD.org
COMMENT=	PHP Scripting Language
WWW=		https://www.php.net/

LICENSE=	PHP301

DEPRECATED=Upstream Security Support ends on 2022-11-28
EXPIRATION_DATE=2022-11-29

USES+=		compiler:c11 tar:xz cpe gnome pkgconfig
CPE_PRODUCT=	php
NO_OPTIONS_SORT=yes

CONFLICTS_INSTALL?=	php[0-9][0-9]${PKGNAMESUFFIX}

.if !defined(PKGNAMESUFFIX)
LIB_DEPENDS=	libpcre2-8.so:devel/pcre2 \
		libargon2.so:security/libargon2

GNU_CONFIGURE=	yes
CONFIGURE_ARGS+=--with-layout=GNU \
		--with-config-file-scan-dir=${PREFIX}/etc/php \
		--disable-all \
		--with-libxml \
		--with-password-argon2=${LOCALBASE} \
		--program-prefix=""
CONFIGURE_ENV=	CPPFLAGS="-I${LOCALBASE}/include"
USES+=		autoreconf:build
USE_GNOME=	libxml2

# PR230207 Allow relocations against read-only segments (override lld default)
LDFLAGS_i386=	-Wl,-z,notext

OPTIONS_DEFINE+=CLI CGI FPM EMBED PHPDBG DEBUG DTRACE IPV6 MYSQLND LINKTHR ZTS
OPTIONS_DEFAULT=CLI CGI FPM EMBED MYSQLND LINKTHR DTRACE
OPTIONS_EXCLUDE_DragonFly=	DTRACE
# ld(1) fails to link probes: Relocations in generic ELF (EM: 0)
OPTIONS_EXCLUDE_aarch64=	DTRACE
# dt_modtext:opensolaris/ib/libdtrace/common/dt_link.c: arm not impemented
OPTIONS_EXCLUDE_armv6=		DTRACE
OPTIONS_EXCLUDE_armv7=		DTRACE
# Bug 197128:  No ASM code for MIPS/MIPS64, disable FPM
OPTIONS_EXCLUDE_mips=		DTRACE FPM
OPTIONS_EXCLUDE_mips64=		DTRACE FPM
OPTIONS_EXCLUDE_powerpc64=	${OPTIONS_EXCLUDE_${ARCH}_${OSREL:R}}
OPTIONS_EXCLUDE_powerpc64_12=	DTRACE
OPTIONS_EXCLUDE_riscv64=	DTRACE
OPTIONS_EXCLUDE_sparc64=	DTRACE
OPTIONS_SUB=	yes

CGI_DESC=	Build CGI version
CLI_DESC=	Build CLI version
EMBED_DESC=	Build embedded library
FPM_DESC=	Build FPM version
LINKTHR_DESC=	Link thread lib (for threaded extensions)
MYSQLND_DESC=	Build with MySQL Native Driver
PHPDBG_DESC=	Interactive PHP debugger
ZTS_DESC=	Force Zend Thread Safety (ZTS) build

CGI_CONFIGURE_OFF=--disable-cgi
CGI_VARS=	PHP_SAPI+=cgi
CLI_CONFIGURE_OFF=--disable-cli
CLI_VARS=	PHP_SAPI+=cli
DEBUG_CONFIGURE_ON=--enable-debug
DTRACE_CONFIGURE_ON=--enable-dtrace
EMBED_CONFIGURE_ON=--enable-embed
EMBED_VARS=	PHP_SAPI+=embed
FPM_CONFIGURE_ON=--enable-fpm \
		--with-fpm-user=${WWWOWN} \
		--with-fpm-group=${WWWGRP}
FPM_VARS=	PHP_SAPI+=fpm USE_RC_SUBR+=php-fpm
IPV6_CONFIGURE_OFF=--disable-ipv6
LINKTHR_LIBS=		-lpthread
MYSQLND_CONFIGURE_ON=--enable-mysqlnd
PHPDBG_CONFIGURE_ON=--enable-phpdbg
PHPDBG_VARS=	PHP_SAPI+=phpdbg
ZTS_CONFIGURE_ENV=	pthreads_working="yes"
ZTS_CONFIGURE_ON=--enable-maintainer-zts

DESTDIRNAME=	INSTALL_ROOT

.include <bsd.port.pre.mk>

.if ${ARCH} == riscv64
CONFIGURE_ARGS+=--without-pcre-jit
.endif

.if defined(PKGNAMEPREFIX)
USES+=		apache:2.2+
.include "${PORTSDIR}/Mk/Uses/apache.mk"
.if ${PORT_OPTIONS:MAP2FILTER}
CONFIGURE_ARGS+=--with-apxs2filter=${APXS}
.else
CONFIGURE_ARGS+=--with-apxs2=${APXS}
.endif
PLIST=		${PKGDIR}/pkg-plist.mod
PKGMESSAGE=	${PKGDIR}/pkg-message.mod
MODULENAME=	libphp7
SHORTMODNAME=	php7
WARNING=	"!!! If you have a threaded Apache, you must build ${PHP_PORT} with ZTS support to enable thread-safety in extensions !!!"
.endif

.if ${PORT_OPTIONS:MPHPDBG} && ${PORT_OPTIONS:MDEBUG}
CONFIGURE_ARGS+=--enable-phpdbg-debug
.endif

.if ${PORT_OPTIONS:MCLI} || ${PORT_OPTIONS:MEMBED}
PLIST_SUB+=	SAPI_INC=""
.else
PLIST_SUB+=	SAPI_INC="@comment "
.endif

CONFIGURE_ENV+=	ac_cv_decimal_fp_supported="no" \
		lt_cv_path_SED="sed"

post-patch:
	@${TOUCH} ${WRKSRC}/ext/php_config.h
	@${REINPLACE_CMD} "s|^\(extension_dir\)|; \1|" ${WRKSRC}/php.ini-*

post-patch-FPM-on:
	@${REINPLACE_CMD} -e "s|^;\(pid\)|\1|;s|^;\(pm\.[a-z_]*_servers\)|\1|" \
		${WRKSRC}/sapi/fpm/php-fpm.conf.in

pre-configure:
	@(cd ${WRKSRC} && ${SETENV} MAKE=${MAKE_CMD} ./buildconf --force)

.if !defined(PKGNAMEPREFIX)
post-build:
	@${ECHO_CMD} "PHP_VER=74" > ${WRKDIR}/php.conf
	@${ECHO_CMD} "PHP_VERSION=${PORTVERSION}" >> ${WRKDIR}/php.conf
	@${ECHO_CMD} "PHP_SAPI=${PHP_SAPI}" >> ${WRKDIR}/php.conf
	@${ECHO_CMD} "PHP_EXT_INC=hash pcre spl" >> ${WRKDIR}/php.conf
	@${ECHO_CMD} -n "PHP_EXT_DIR=" >> ${WRKDIR}/php.conf
	@${SH} ${WRKSRC}/scripts/php-config --extension-dir | ${SED} -ne 's,^${PREFIX}/lib/php/,,p' >> ${WRKDIR}/php.conf

test: build
	@(cd ${WRKSRC} && ${MAKE} test)

post-install:
	${INSTALL_DATA} ${WRKSRC}/php.ini-development ${WRKSRC}/php.ini-production \
		${WRKDIR}/php.conf ${STAGEDIR}/${PREFIX}/etc
.else
do-install:
	${MKDIR} ${STAGEDIR}${PREFIX}/${APACHEMODDIR}
	${INSTALL_LIB} ${WRKSRC}/libs/${MODULENAME}.so \
		${STAGEDIR}${PREFIX}/${APACHEMODDIR}
.endif

.else
COMMENT=	The ${PHP_MODNAME} shared extension for php

USES+=	php:ext,noflavors
PHP_MODNAME=	${PKGNAMESUFFIX:S/-//}
PHP_VER=	74
IGNORE_WITH_PHP= 72 73

EXTSUBDIR=	${DISTNAME}/ext/${PHP_MODNAME}
WRKSRC=		${WRKDIR}/${EXTSUBDIR}
PATCHDIR=	${.CURDIR}/files
PLIST=		${NONEXISTENT}

EXTRACT_AFTER_ARGS?=	${EXTSUBDIR}

.if ${PHP_MODNAME} == "bcmath"
CONFIGURE_ARGS+=--enable-bcmath

PHP_HEADER_DIRS=libbcmath/src
.endif

.if ${PHP_MODNAME} == "bz2"
CONFIGURE_ARGS+=--with-bz2=/usr
.endif

.if ${PHP_MODNAME} == "calendar"
CONFIGURE_ARGS+=--enable-calendar
.endif

.if ${PHP_MODNAME} == "ctype"
CONFIGURE_ARGS+=--enable-ctype
.endif

.if ${PHP_MODNAME} == "curl"
LIB_DEPENDS+=	libcurl.so:ftp/curl

CONFIGURE_ARGS+=--with-curl=${LOCALBASE}
USES+=		pkgconfig
.endif

.if ${PHP_MODNAME} == "dba"
CONFIGURE_ARGS+=--enable-dba

OPTIONS_DEFINE=	CDB DB4 GDBM QDBM TOKYO INIFILE FLATFILE LMDB
OPTIONS_DEFAULT=CDB INIFILE FLATFILE

CDB_DESC=	cdb database support
DB4_DESC=	Berkeley DB4 support
GDBM_DESC=	GDBM database support
QDBM_DESC=	QDBM database support
TOKYO_DESC=	Tokyo Cabinet database support
INIFILE_DESC=	INI file support
FLATFILE_DESC=	flatfile support
LMDB_DESC=	LMDB database support

PHP_HEADER_DIRS=	libcdb libflatfile libinifile
.endif

.if ${PHP_MODNAME} == "dom"
CONFIGURE_ARGS+=--enable-dom \
		--with-libxml-dir=${LOCALBASE}

USES+=		pkgconfig
USE_GNOME=	libxml2
.endif

.if ${PHP_MODNAME} == "enchant"
LIB_DEPENDS+=   libenchant.so:textproc/enchant
CONFIGURE_ARGS+=--with-enchant=${LOCALBASE}
USES+=		pkgconfig
.endif

.if ${PHP_MODNAME} == "exif"
CONFIGURE_ARGS+=--enable-exif
.endif

.if ${PHP_MODNAME} == "ffi"
LIB_DEPENDS=	libffi.so:devel/libffi
CONFIGURE_ARGS+=--enable-ffi
.endif

.if ${PHP_MODNAME} == "fileinfo"
CONFIGURE_ARGS+=--enable-fileinfo \
		--with-pcre-dir=${LOCALBASE}

PHP_HEADER_DIRS=	libmagic
.endif

.if ${PHP_MODNAME} == "filter"
CONFIGURE_ARGS+=--enable-filter \
		--with-pcre-dir=${LOCALBASE}
.endif

.if ${PHP_MODNAME} == "ftp"
CONFIGURE_ARGS+=--enable-ftp

CONFIGURE_ENV+=	OPENSSL_CFLAGS="-I${OPENSSLINC}" \
		OPENSSL_LIBS="-L${OPENSSLLIB} -lssl -lcrypto" \
		PHP_OPENSSL=yes

LDFLAGS+=	-L${OPENSSLLIB} -lcrypto -lssl
USES+=		ssl pkgconfig
.endif

.if ${PHP_MODNAME} == "gd"
LIB_DEPENDS=	libfreetype.so:print/freetype2 \
		libpng.so:graphics/png
USES+=		jpeg

CONFIGURE_ARGS+=--enable-gd \
		--with-external-gd

EXTRACT_AFTER_ARGS=--no-same-owner --no-same-permissions --exclude libgd \
		php-${DISTVERSION}/ext/gd

OPTIONS_DEFINE=TRUETYPE JIS WEBP X11
OPTIONS_DEFAULT=TRUETYPE

TRUETYPE_DESC=	Enable TrueType string function
JIS_DESC=	Enable JIS-mapped Japanese font support
WEBP_DESC=  Enable WebP image format support
X11_DESC=	Enable XPM support

.endif

.if ${PHP_MODNAME} == "gettext"
CONFIGURE_ARGS+=--with-gettext=${LOCALBASE}

USES+=		gettext
.endif

.if ${PHP_MODNAME} == "gmp"
LIB_DEPENDS+=	libgmp.so:math/gmp

CONFIGURE_ARGS+=--with-gmp=${LOCALBASE}
.endif

.if ${PHP_MODNAME} == "iconv"
CONFIGURE_ARGS+=--with-iconv=${LOCALBASE}

USES+=	iconv:translit
.endif

.if ${PHP_MODNAME} == "imap"
PHP_MOD_PRIO=	30
OPTIONS_RADIO=  CLIENT
OPTIONS_RADIO_CLIENT=	CCLIENT PANDA
OPTIONS_DEFAULT=PANDA
CCLIENT_DESC=	Uses the original but unmaintained cclient
PANDA_DESC=	Uses the forked panda-cclient instead of the original cclient

CONFIGURE_ARGS+=--with-imap=${LOCALBASE} \
		--with-pcre-dir=${LOCALBASE} \
		--with-imap-ssl=${OPENSSLBASE}

CONFIGURE_ENV+=	OPENSSL_CFLAGS="-I${OPENSSLINC}" \
		OPENSSL_LIBS="-L${OPENSSLLIB} -lssl -lcrypto" \
		PHP_OPENSSL=yes

LDFLAGS+=	-L${OPENSSLLIB} -lcrypto -lssl
USES+=		ssl
.endif

.if ${PHP_MODNAME} == "intl"
CONFIGURE_ARGS+=--with-intl=${LOCALBASE}
LIB_DEPENDS=	libicui18n.so:devel/icu
USES+=		pkgconfig
.endif

.if ${PHP_MODNAME} == "json"
CONFIGURE_ARGS+=--enable-json
BUILD_DEPENDS=  re2c:devel/re2c
.endif

.if ${PHP_MODNAME} == "ldap"
CONFIGURE_ARGS+=--with-ldap=${LOCALBASE} --with-ldap-sasl=${LOCALBASE}

USE_OPENLDAP=	yes
.endif

.if ${PHP_MODNAME} == "mbstring"

CONFIGURE_ARGS+=--enable-mbstring \
		--with-pcre-dir=${LOCALBASE}

OPTIONS_DEFINE=	REGEX
OPTIONS_DEFAULT=REGEX

REGEX_DESC=	Enable multibyte regex support

PHP_HEADER_DIRS=libmbfl libmbfl/filters libmbfl/mbfl libmbfl/nls
.endif

.if ${PHP_MODNAME} == "mysqli"
OPTIONS_DEFINE=	MYSQLND
OPTIONS_DEFAULT=MYSQLND

MYSQLND_DESC=	Use MySQL Native Driver
.endif

.if ${PHP_MODNAME} == "odbc"
LIB_DEPENDS+=	libodbc.so:databases/unixODBC

CONFIGURE_ARGS+=--enable-odbc \
		--with-unixODBC=${LOCALBASE}
.endif

.if ${PHP_MODNAME} == "openssl"
USES+=			ssl
CONFIGURE_ARGS+=	--with-openssl
CONFIGURE_ENV+=		OPENSSL_CFLAGS="-I${OPENSSLINC}" \
			OPENSSL_LIBS="-L${OPENSSLLIB} -lssl -lcrypto"
.endif

.if ${PHP_MODNAME} == "opcache"
# This is needed by Zend extensions, keep before everything.
PHP_MOD_PRIO=	10
CONFIGURE_ARGS+=--enable-opcache
USES+=	localbase
.endif

.if ${PHP_MODNAME} == "pcntl"
CONFIGURE_ARGS+=--enable-pcntl
.endif

.if ${PHP_MODNAME} == "pdo"
CONFIGURE_ARGS+=--enable-pdo
.endif

.if ${PHP_MODNAME} == "pdo_dblib"
CONFIGURE_ARGS+=--with-pdo-dblib=${LOCALBASE}

USE_PHP=	pdo:build

OPTIONS_DEFINE=	MSSQL

MSSQL_DESC=	Enable Microsoft SQL Server support
.endif

.if ${PHP_MODNAME} == "pdo_firebird"
CONFIGURE_ARGS+=--with-pdo-firebird=${LOCALBASE}

USES+=	firebird
USE_PHP=	pdo:build
.endif

.if ${PHP_MODNAME} == "pdo_odbc"
LIB_DEPENDS+=	libodbc.so:databases/unixODBC
CONFIGURE_ARGS+=--with-pdo-odbc=unixODBC,${LOCALBASE}

USE_PHP=	pdo:build
.endif

.if ${PHP_MODNAME} == "pdo_mysql"
OPTIONS_DEFINE=	MYSQLND
OPTIONS_DEFAULT=MYSQLND

MYSQLND_DESC=	Use MySQL Native Driver

USE_PHP=	pdo:build
.endif

.if ${PHP_MODNAME} == "pdo_pgsql"
USES+=		pgsql

CONFIGURE_ARGS+=--with-pdo-pgsql=${LOCALBASE}

USE_PHP=	pdo:build
.endif

.if ${PHP_MODNAME} == "pdo_sqlite"
USES+=		localbase sqlite
CONFIGURE_ARGS+=--with-pdo-sqlite=${LOCALBASE}

USE_PHP=	pdo:build
.endif

.if ${PHP_MODNAME} == "pgsql"
USES+=		pgsql

CONFIGURE_ARGS+=--with-pgsql=${LOCALBASE}
.endif

.if ${PHP_MODNAME} == "phar"
CONFIGURE_ARGS+=--enable-phar \
		--with-pcre-dir=${LOCALBASE}
.endif

.if ${PHP_MODNAME} == "posix"
CONFIGURE_ARGS+=--enable-posix
.endif

.if ${PHP_MODNAME} == "pspell"
LIB_DEPENDS+=	libaspell.so:textproc/aspell

CONFIGURE_ARGS+=--with-pspell=${LOCALBASE}
.endif

.if ${PHP_MODNAME} == "readline"
CONFIGURE_ARGS+=--with-readline=/usr
USES+=		readline
.endif

.if ${PHP_MODNAME} == "session"
CONFIGURE_ARGS+=--enable-session
PHP_MOD_PRIO=	18
.endif

.if ${PHP_MODNAME} == "shmop"
CONFIGURE_ARGS+=--enable-shmop
.endif

.if ${PHP_MODNAME} == "simplexml"
CONFIGURE_ARGS+=--enable-simplexml \
		--with-pcre-dir=${LOCALBASE} \
		--with-libxml-dir=${LOCALBASE}

USES+=		pkgconfig
USE_GNOME=	libxml2
.endif

.if ${PHP_MODNAME} == "snmp"
CONFIGURE_ARGS+=--with-snmp=${LOCALBASE} \
		--with-openssl-dir=${OPENSSLBASE}

CONFIGURE_ENV+=	ac_cv_buggy_snprint_value="no"

LIB_DEPENDS+=	libnetsnmp.so:net-mgmt/net-snmp
LDFLAGS+=	-L${OPENSSLLIB} -lcrypto -lssl
USES+=	ssl
.endif

.if ${PHP_MODNAME} == "soap"
CONFIGURE_ARGS+=--enable-soap \
		--with-libxml-dir=${LOCALBASE}

USES+=		pkgconfig
USE_GNOME=	libxml2
.endif

.if ${PHP_MODNAME} == "sockets"
CONFIGURE_ARGS+=--enable-sockets
.endif

.if ${PHP_MODNAME} == "sodium"
CONFIGURE_ARGS+=	--with-sodium=${LOCALBASE}
LIB_DEPENDS+=	libsodium.so:security/libsodium
.endif

.if ${PHP_MODNAME} == "sqlite3"
USES+=		localbase pkgconfig sqlite
CONFIGURE_ARGS+=--with-sqlite3=${LOCALBASE}
.endif

.if ${PHP_MODNAME} == "sysvmsg"
CONFIGURE_ARGS+=--enable-sysvmsg
.endif

.if ${PHP_MODNAME} == "sysvsem"
CONFIGURE_ARGS+=--enable-sysvsem
.endif

.if ${PHP_MODNAME} == "sysvshm"
CONFIGURE_ARGS+=--enable-sysvshm
.endif

.if ${PHP_MODNAME} == "tidy"
CONFIGURE_ARGS+=--with-tidy=${LOCALBASE}

LIB_DEPENDS+=	libtidy.so:www/tidy-lib
.endif

.if ${PHP_MODNAME} == "tokenizer"
CONFIGURE_ARGS+=--enable-tokenizer
.endif

.if ${PHP_MODNAME} == "xml"
CONFIGURE_ARGS+=--enable-xml \
		--with-libxml-dir=${LOCALBASE}

USES+=		pkgconfig
USE_GNOME=	libxml2
.endif

.if ${PHP_MODNAME} == "xmlreader"
CONFIGURE_ARGS+=--enable-xmlreader \
		--with-libxml-dir=${LOCALBASE}

USE_GNOME=	libxml2

USE_PHP=	dom:build
.endif

.if ${PHP_MODNAME} == "xmlrpc"
CONFIGURE_ARGS+=--with-xmlrpc \
		--with-libxml-dir=${LOCALBASE} \
		--with-iconv-dir=${LOCALBASE}

USES+=		iconv:translit
USE_GNOME=	libxml2

PHP_HEADER_DIRS=libxmlrpc

USE_PHP=	xml:build
.endif

.if ${PHP_MODNAME} == "xmlwriter"
CONFIGURE_ARGS+=--enable-xmlwriter \
                --with-libxml-dir=${LOCALBASE}

USES+=		pkgconfig
USE_GNOME=      libxml2
.endif

.if ${PHP_MODNAME} == "xsl"
CONFIGURE_ARGS+=--with-xsl=${LOCALBASE}

USE_GNOME=	libxslt
USE_PHP=	dom:build xml:build
.endif

.if ${PHP_MODNAME} == "zip"
LIB_DEPENDS+=	libzip.so:archivers/libzip

CONFIGURE_ARGS+=--enable-zip \
		--with-zlib-dir=/usr \
		--with-pcre-dir=${LOCALBASE} \
		--with-libzip=${LOCALBASE}
USES+=		pkgconfig
.endif

.if ${PHP_MODNAME} == "zlib"
CONFIGURE_ARGS+=--with-zlib=/usr
USES+=		pkgconfig
.endif

.include <bsd.port.options.mk>

.if ${PHP_MODNAME} == "dba"
.	if empty(PORT_OPTIONS:MCDB)
CONFIGURE_ARGS+=--without-cdb
.	endif
.	if ${PORT_OPTIONS:MDB4}
CONFIGURE_ARGS+=--with-db4=${LOCALBASE}

USES+=			bdb
WITH_BDB_HIGHEST=	yes
.	endif
.	if ${PORT_OPTIONS:MGDBM}
LIB_DEPENDS+=	libgdbm.so:databases/gdbm

CONFIGURE_ARGS+=--with-gdbm=${LOCALBASE}
.	endif
.	if ${PORT_OPTIONS:MQDBM}
LIB_DEPENDS+=	libqdbm.so:databases/qdbm

CONFIGURE_ARGS+=--with-qdbm=${LOCALBASE}
.	endif
.	if ${PORT_OPTIONS:MTOKYO}
LIB_DEPENDS+=	libtokyocabinet.so:databases/tokyocabinet

CONFIGURE_ARGS+=--with-tcadb=${LOCALBASE}
.	endif
.	if empty(PORT_OPTIONS:MINIFILE)
CONFIGURE_ARGS+=--disable-inifile
.	endif
.	if empty(PORT_OPTIONS:MFLATFILE)
CONFIGURE_ARGS+=--disable-flatfile
.	endif

.	if ${PORT_OPTIONS:MLMDB}
LIB_DEPENDS+=	liblmdb.so:databases/lmdb
CONFIGURE_ARGS+=	--with-lmdb=${LOCALBASE}
.endif
.endif

.if ${PHP_MODNAME} == "mysqli"
.	if ${PORT_OPTIONS:MMYSQLND}
CONFIGURE_ARGS+=--with-mysqli=mysqlnd
.	else
CONFIGURE_ARGS+=--with-mysqli=${LOCALBASE}/bin/mysql_config
USES+=	mysql
.	endif
.endif

.if ${PHP_MODNAME} == "pdo_mysql"
.	if ${PORT_OPTIONS:MMYSQLND}
CONFIGURE_ARGS+=--with-pdo-mysql=mysqlnd
.	else
CONFIGURE_ARGS+=--with-pdo-mysql=${LOCALBASE} \
		--with-zlib-dir=/usr
USES+=	mysql
.	endif
.endif

.if ${PHP_MODNAME} == "gd"
.	if ${PORT_OPTIONS:MJIS}
CONFIGURE_ARGS+=--enable-gd-jis-conv
.	endif
.   if ${PORT_OPTIONS:MWEBP}
LIB_DEPENDS+=   libwebp.so:graphics/webp

CONFIGURE_ARGS+=--with-webp
.   endif
.	if ${PORT_OPTIONS:MX11}
USES+=		xorg
USE_XORG=	xpm

CONFIGURE_ARGS+=--with-xpm
.	endif
.endif

.include <bsd.port.pre.mk>

.if ${PHP_MODNAME} == "imap"
.	if ${PORT_OPTIONS:MPANDA}
LIB_DEPENDS+=	libc-client.so.2010:mail/panda-cclient
.	else
LIB_DEPENDS+=	libc-client4.so.9:mail/cclient
.	endif
.endif

.if ${PHP_MODNAME} == "mbstring"
.	if ${PORT_OPTIONS:MREGEX}
LIB_DEPENDS+=	libonig.so:devel/oniguruma
CONFIGURE_ARGS+=--with-onig=${LOCALBASE}
.	else
CONFIGURE_ARGS+=--disable-mbregex
.	endif
.endif

.if ${PHP_MODNAME} == "openssl" || ${PHP_MODNAME} == "sqlite3"
post-extract:
	@${MV} ${WRKSRC}/config0.m4 ${WRKSRC}/config.m4
.endif

.if ${PHP_MODNAME} == "pdo_dblib"
.	if ${PORT_OPTIONS:MMSSQL}
LIB_DEPENDS+=	libsybdb.so:databases/freetds
.	else
LIB_DEPENDS+=	libct.so:databases/freetds
.	endif
.endif

.if ${PHP_MODNAME} == "xml"
post-extract:
	@${MKDIR} ${WRKSRC}/ext/xml
	@${CP} ${WRKSRC}/expat_compat.h ${WRKSRC}/ext/xml
.endif

.if ${PHP_MODNAME} == "zlib"
post-extract:
	@${MV} ${WRKSRC}/config0.m4 ${WRKSRC}/config.m4
.endif
.endif
.include <bsd.port.post.mk>
