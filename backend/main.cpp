#include "crow_all.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ✅ Middleware to handle CORS
struct CORS {
    struct context {};

    void before_handle(crow::request& req, crow::response& res, context&) {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type");

        if (req.method == crow::HTTPMethod::Options) {
            res.code = 204;
            res.end();
        }
    }

    void after_handle(crow::request&, crow::response& res, context&) {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Headers", "Content-Type");
    }
};

// ✅ Utility to split string by comma
std::vector<std::string> split_subjects(const std::string& input) {
    std::vector<std::string> result;
    std::stringstream ss(input);
    std::string token;
    while (std::getline(ss, token, ',')) {
        if (!token.empty()) result.push_back(token);
    }
    return result;
}

// ✅ Dummy study plan generator
json generate_study_plan(const std::string& name, const std::vector<std::string>& subjects,
                         const std::string& exam_date_str, int hours_per_day) {
    json plan = json::array();

    // Get today's date
    std::time_t t = std::time(nullptr);
    std::tm* today = std::localtime(&t);

    for (const auto& subject : subjects) {
        for (int i = 0; i < 3; ++i) {
            std::tm task_day = *today;
            task_day.tm_mday += i;
            std::mktime(&task_day);

            char date_buf[11];
            std::strftime(date_buf, sizeof(date_buf), "%Y-%m-%d", &task_day);

            plan.push_back({
                {"date", date_buf},
                {"subject", subject},
                {"topic", "Topic " + std::to_string(i + 1)},
                {"hours", hours_per_day / 3}
            });
        }
    }

    return plan;
}

int main() {
    crow::App<CORS> app;

    CROW_ROUTE(app, "/generate_plan").methods("POST"_method)([](const crow::request& req) {
        try {
            auto body = json::parse(req.body);

            std::string name = body["name"].get<std::string>();
            std::string subjects_str = body["subjects"].get<std::string>();
            std::string exam_date = body["exam_date"].get<std::string>();
            int hours_per_day = std::stoi(body["hours_per_day"].get<std::string>());

            auto subjects = split_subjects(subjects_str);

            json response;
            response["plan"] = generate_study_plan(name, subjects, exam_date, hours_per_day);

            crow::response res;
            res.code = 200;
            res.set_header("Content-Type", "application/json");
            res.write(response.dump());
            return res;
        } catch (const std::exception& e) {
            crow::response res;
            res.code = 500;
            res.write(std::string("Error: ") + e.what());
            return res;
        }
    });

    app.port(18080).multithreaded().run();
}
