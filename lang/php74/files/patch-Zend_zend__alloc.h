--- Zend/zend_alloc.h.orig	2026-10-29 09:47:11 UTC
+++ Zend/zend_alloc.h
@@ -402,6 +402,8 @@ size_t zend_mm_globals_size(void);
 size_t zend_mm_globals_size(void);
 #endif
 
+ZEND_API ZEND_COLD ZEND_NORETURN void zend_mm_safe_error_ex(const char *format, ...);
+
 END_EXTERN_C()
 
 #endif
