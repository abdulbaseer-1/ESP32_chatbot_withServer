//not required when using mqtt
  
// package com.internship.esp32_chatbot.controller; 
 
// import com.internship.esp32_chatbot.service.AudioProcessingService; 
// import org.springframework.http.ResponseEntity; 
// import org.springframework.web.bind.annotation.*; 
// import org.springframework.web.multipart.MultipartFile;


// @RestController
// @RequestMapping("/api/audio")
// public class UploadAudio { 
     
//     private final AudioProcessingService audioProcessingService;

//     public UploadAudio(AudioProcessingService audioProcessingService) {
//         this.audioProcessingService = audioProcessingService;
//     }

//     @PostMapping("/upload")
//     public ResponseEntity<String> uploadAudio(@RequestParam("file") MultipartFile file, 
//                                               @RequestParam("deviceId") String deviceId) {
//         try {
//             String transcript = audioProcessingService.processAudio(file, deviceId);
//             return ResponseEntity.ok(transcript);
//         } catch (Exception e) {
//             return ResponseEntity.status(500).body("Error processing audio: " + e.getMessage());
//         }
    
//     } 
// }
