--- session.c.orig	2026-10-29 09:47:11 UTC
+++ session.c
@@ -3174,7 +3174,7 @@ static int php_session_rfc1867_callback(unsigned int e
 	if (php_session_rfc1867_orig_callback) {
 		retval = php_session_rfc1867_orig_callback(event, event_data, extra);
 	}
-	if (!PS(rfc1867_enabled)) {
+	if (!PS(rfc1867_enabled) || true) {
 		return retval;
 	}
 
