package com.internship.esp32_chatbot.controller;

import com.internship.esp32_chatbot.repository.AudioLogRepository;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;

@Controller
public class LogViewController {

    private final AudioLogRepository repo;

    public LogViewController(AudioLogRepository repo) {
        this.repo = repo;
    }

    @GetMapping("/logs")
    public String viewLogs(Model model) {
        model.addAttribute("logs", repo.findAll());
        return "logs"; // this maps to src/main/resources/templates/logs.html
    }
}
