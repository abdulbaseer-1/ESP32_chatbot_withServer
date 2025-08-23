package com.internship.esp32_chatbot.controller; 
 
import org.springframework.stereotype.Controller; 
import org.springframework.web.bind.annotation.GetMapping;
 
@Controller
public class HomePageController {
     
    @GetMapping("/") 
    public String home() {
        // This method will return the name of the view to be rendered for the home page
        return "home"; // Assuming there is a view named 'home.html' or 'home.jsp'
    }
}
