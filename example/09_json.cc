//
// Created by Chanchan on 11/14/21.
//

#include <workflow/WFFacilities.h>

#include <csignal>

#include "HttpServer.h"
#include "HttpMsg.h"
#include "json.hpp"

using namespace wfrest;

static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int signo)
{
    wait_group.done();
}

int main()
{
    signal(SIGINT, sig_handler);

    HttpServer svr;

    // curl -v http://ip:port/json1
    svr.Get("/json1", [](const HttpReq *req, HttpResp *resp)
    {
        Json json;
        json["test"] = 123;
        json["json"] = "test json";
        resp->Json(json);
    });

    // curl -v http://ip:port/json2
    svr.Get("/json2", [](const HttpReq *req, HttpResp *resp)
    {
        std::string valid_text = R"(
        {
            "numbers": [1, 2, 3]
        }
        )";
        resp->Json(valid_text);
    });

    // curl -v http://ip:port/json3
    svr.Get("/json3", [](const HttpReq *req, HttpResp *resp)
    {
        std::string invalid_text = R"(
        {
            "strings": ["extra", "comma", ]
        }
        )";
        resp->Json(invalid_text);
    });

    // recieve json
    //   curl -X POST http://ip:port/json4
    //   -H 'Content-Type: application/json'
    //   -d '{"login":"my_login","password":"my_password"}'
    svr.Post("/json4", [](const HttpReq *req, HttpResp *resp)
    {
        if(req->content_type != APPLICATION_JSON)
        {
            resp->String("NOT APPLICATION_JSON");
            return;
        }
        fprintf(stderr, "Json : %s", req->json.dump(4).c_str());
    });

    if (svr.start(9001) == 0)
    {
        wait_group.wait();
        svr.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}