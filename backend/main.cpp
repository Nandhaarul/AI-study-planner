#include "crow_all.h"
#include <iostream>
#include <fstream>

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/generate_plan").methods("POST"_method)([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body)
        {
            return crow::response(400, "Invalid JSON");
        }

        std::string name = body["name"].s();
        std::string subjects = body["subjects"].s();
        std::string exam_date = body["exam_date"].s();
        std::string hours_per_day = body["hours_per_day"].s();
        std::string skills = body["skills"].s();

        std::cout << "✅ Received Input:\n";
        std::cout << "Name: " << name << "\n";
        std::cout << "Subjects: " << subjects << "\n";
        std::cout << "Exam Date: " << exam_date << "\n";
        std::cout << "Hours/Day: " << hours_per_day << "\n";
        std::cout << "Skills: " << skills << "\n";

        // Optional: save to file
        std::ofstream out("user_input.csv", std::ios::app);
        out << name << "," << subjects << "," << exam_date << "," << hours_per_day << "," << skills << "\n";
        out.close();

        crow::json::wvalue response_body;
        response_body["status"] = "success";
        response_body["message"] = "Form received successfully!";

        return crow::response(200, response_body);
    });

    app.port(18080).multithreaded().run();
}
