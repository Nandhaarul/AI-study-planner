#include "crow_all.h"
#include <iostream>
#include <fstream>

// ✅ Correct CORS Middleware
struct CORS {
    struct context {};

    void before_handle(crow::request& req, crow::response& res, context&) {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type");

        // ✅ Handle preflight request
        if (req.method == crow::HTTPMethod::Options) {
            res.code = 204;
            res.end(); // 👈 MUST call end() to stop further processing
        }
    }

    void after_handle(crow::request&, crow::response& res, context&) {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Headers", "Content-Type");
    }
};

int main() {
    crow::App<CORS> app;

    CROW_ROUTE(app, "/generate_plan").methods("POST"_method)
    ([](const crow::request& req) {
        std::cout << "🔄 POST /generate_plan called\n";

        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");

        std::string name = body["name"].s();
        std::string subjects = body["subjects"].s();
        std::string exam_date = body["exam_date"].s();
        std::string hours_per_day = body["hours_per_day"].s();

        std::cout << "✅ Received Input:\n";
        std::cout << "Name: " << name << "\n";
        std::cout << "Subjects: " << subjects << "\n";
        std::cout << "Exam Date: " << exam_date << "\n";
        std::cout << "Hours/Day: " << hours_per_day << "\n";

        std::ofstream out("user_input.csv", std::ios::app);
        out << name << "," << subjects << "," << exam_date << "," << hours_per_day << "\n";
        out.close();

        crow::json::wvalue::list plan_items;
        plan_items.push_back(crow::json::wvalue{
            {"date", "2025-07-11"},
            {"subject", subjects},
            {"topic", "Arrays and Pointers"},
            {"hours", std::stoi(hours_per_day)}
        });
        plan_items.push_back(crow::json::wvalue{
            {"date", "2025-07-12"},
            {"subject", subjects},
            {"topic", "Sorting & Searching"},
            {"hours", std::stoi(hours_per_day)}
        });

        crow::json::wvalue res;
        res["status"] = "success";
        res["plan"] = std::move(plan_items);

        return crow::response(200, res);
    });

    app.port(18080).multithreaded().run();
}
