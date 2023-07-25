from http.server import HTTPServer, BaseHTTPRequestHandler
import ssl


class SimpleHTTPRequestHandler(BaseHTTPRequestHandler):

    def do_GET(self):
        self.send_response(200)
        self.end_headers()
        self.wfile.write(b'Hello, world!')
        self.wfile.close()


httpd = HTTPServer(('0.0.0.0', 4443), SimpleHTTPRequestHandler)

context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
context.verify_mode = ssl.CERT_REQUIRED
context.options &= ~ssl.OP_NO_SSLv3
context.check_hostname = Falses
context.set_default_verify_paths()
context.load_cert_chain(certfile="server.pem", keyfile="server.key")
context.load_verify_locations(cafile="chain.pem")
httpd.socket = context.wrap_socket(httpd.socket)
print(context.get_ca_certs())
httpd.serve_forever()
