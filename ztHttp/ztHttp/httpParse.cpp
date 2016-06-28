#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     //for getopt, fork
#include <string.h>     //for strcat
//for struct evkeyvalq
#include <sys/queue.h>
#include <event.h>
//for http
#include <evhttp.h>
#include <signal.h>
#define MYHTTPD_SIGNATURE   "myhttpd v 0.0.1"
#
    //����ģ��
void httpd_handler(struct evhttp_request *req, void *arg) {
    char output[2048] = "\0";
    char tmp[1024];

    //��ȡ�ͻ��������URI(ʹ��evhttp_request_uri��ֱ��req->uri)
    const char *uri;
    uri = evhttp_request_uri(req);
    sprintf(tmp, "uri=%s\n", uri);
    strcat(output, tmp);

    sprintf(tmp, "uri=%s\n", req->uri);
    strcat(output, tmp);
    //decoded uri
    char *decoded_uri;
    decoded_uri = evhttp_decode_uri(uri);
    sprintf(tmp, "decoded_uri=%s\n", decoded_uri);
    strcat(output, tmp);

    //����URI�Ĳ���(��GET�����Ĳ���)
    struct evkeyvalq params;
    evhttp_parse_query(decoded_uri, &params);
    sprintf(tmp, "q=%s\n", evhttp_find_header(&params, "q"));
    strcat(output, tmp);
    sprintf(tmp, "s=%s\n", evhttp_find_header(&params, "s"));
    strcat(output, tmp);
    free(decoded_uri);

    //��ȡPOST����������
    char *post_data = (char *) EVBUFFER_DATA(req->input_buffer);
    sprintf(tmp, "post_data=%s\n", post_data);
    strcat(output, tmp);

        /*
        ����ģ����Ը���GET/POST�Ĳ���ִ����Ӧ������Ȼ�󽫽�����
        ...
        */

        /* ������ͻ��� */

    //HTTP header
    evhttp_add_header(req->output_headers, "Server", MYHTTPD_SIGNATURE);
    evhttp_add_header(req->output_headers, "Content-Type", "text/plain; charset=UTF-8");
    evhttp_add_header(req->output_headers, "Connection", "close");
    //���������
    struct evbuffer *buf;
    buf = evbuffer_new();
    evbuffer_add_printf(buf, "It works!\n%s\n", output);
    evhttp_send_reply(req, HTTP_OK, "OK", buf);
    evbuffer_free(buf);

}
    void show_help() {
        char *help = "written by Min (http://54min.com)\n\n"
            "-l <ip_addr> interface to listen on, default is 0.0.0.0\n"
            "-p <num>     port number to listen on, default is 1984\n"
            "-d           run as a deamon\n"
            "-t <second>  timeout for a http request, default is 120 seconds\n"
            "-h           print this help and exit\n"
            "\n";
        fprintf(stderr, help);
    }
    //������̷���SIGTERM/SIGHUP/SIGINT/SIGQUIT��ʱ����ֹevent���¼�����ѭ��
    void signal_handler(int sig) {
        switch (sig) {
            case SIGTERM:
            case SIGHUP:
            case SIGQUIT:
            case SIGINT:
                event_loopbreak();  //��ֹ����event_dispatch()���¼�����ѭ����ִ��֮��Ĵ���
                break;
        }
    }

    int main11(int argc, char *argv[]) {
        //�Զ����źŴ�������
        signal(SIGHUP, signal_handler);
        signal(SIGTERM, signal_handler);
        signal(SIGINT, signal_handler);
        signal(SIGQUIT, signal_handler);

        //Ĭ�ϲ���
        char *httpd_option_listen = "0.0.0.0";
        int httpd_option_port = 8080;
        int httpd_option_daemon = 0;
        int httpd_option_timeout = 120; //in seconds

        //��ȡ����
        int c;
        while ((c = getopt(argc, argv, "l:p:dt:h")) != -1) {
            switch (c) {
            case 'l' :
                httpd_option_listen = optarg;
                break;
            case 'p' :
                httpd_option_port = atoi(optarg);
                break;
            case 'd' :
                httpd_option_daemon = 1;
                break;
            case 't' :
                httpd_option_timeout = atoi(optarg);
                 break;
            case 'h' :
            default :
                    show_help();
                    exit(EXIT_SUCCESS);
            }
        }

        //�ж��Ƿ�������-d����daemon����
        if (httpd_option_daemon) {
            pid_t pid;
            pid = fork();
            if (pid < 0) {
                perror("fork failed");
                exit(EXIT_FAILURE);
            }
            if (pid > 0) {
                //�����ӽ��̳ɹ����˳�������
                exit(EXIT_SUCCESS);
            }
        }

        /* ʹ��libevent����HTTP Server */

        //��ʼ��event API
        event_init();

        //����һ��http server
        struct evhttp *httpd;
        httpd = evhttp_start(httpd_option_listen, httpd_option_port);
        evhttp_set_timeout(httpd, httpd_option_timeout);

        //ָ��generic callback
        evhttp_set_gencb(httpd, httpd_handler, NULL);
        //Ҳ����Ϊ�ض���URIָ��callback
        //evhttp_set_cb(httpd, "/", specific_handler, NULL);

        //ѭ������events
        event_dispatch();

        evhttp_free(httpd);
        return 0;
    }
