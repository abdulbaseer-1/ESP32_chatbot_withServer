package com.internship.esp32_chatbot.controller;
 
import org.springframework.stereotype.Controller; 
import org.springframework.web.bind.annotation.GetMapping;
 
@Controller
public class AboutPageController {
     
    @GetMapping("/about") 
    public String viewAbout(){ 
        return "about";
    }
}
