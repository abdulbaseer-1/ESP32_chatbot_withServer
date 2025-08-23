// package com.internship.esp32_chatbot.service;

// import com.internship.esp32_chatbot.repository.AudioLogRepository;
// import org.springframework.stereotype.Service;
// import org.springframework.util.LinkedMultiValueMap;
// import org.springframework.util.MultiValueMap;
// import org.springframework.beans.factory.annotation.Value;
// import org.springframework.core.io.ByteArrayResource;
// import org.springframework.http.*;
// import org.springframework.web.client.RestTemplate;
// import com.fasterxml.jackson.databind.JsonNode;
// import com.fasterxml.jackson.databind.ObjectMapper;
// import java.util.List;
// import java.util.Map;

// @Service
// public class AudioProcessingService { 
     
//     @Value("${ai.api-key}")
//     private String aiApiKey;

//     private final LogService logService;
//     private final RestTemplate restTemplate;
//     private final ObjectMapper objectMapper;

//     public AudioProcessingService(LogService logService) {
//         this.logService = logService;
//         this.restTemplate = new RestTemplate();
//         this.objectMapper = new ObjectMapper();
//     }

//     public String processAudioFromMQTT(byte[] audioBytes, String deviceId) throws Exception {
//         // --- 1. Transcribe audio using Whisper API ---
//         String transcript = transcribeAudio(audioBytes);

//         // --- 2. Log the transcript ---
//         logService.saveLog(deviceId, transcript);

//         // --- 3. Forward transcript to GPT API ---
//         String aiReply = queryGPT(transcript);

//         return aiReply;
//     }

//     private String transcribeAudio(byte[] audioBytes) throws Exception {
//         String apiURL = "https://api.assemblyai.com/v2/transcript";//https://api.openai.com/v1/audio/transcriptions 

//         HttpHeaders headers = new HttpHeaders();
//         headers.setContentType(MediaType.MULTIPART_FORM_DATA);
//         headers.setBearerAuth(aiApiKey);

//         HttpHeaders fileHeaders = new HttpHeaders();
//         fileHeaders.setContentDispositionFormData("file", "audio.wav");
//         fileHeaders.setContentType(MediaType.valueOf("audio/wav"));

//         HttpEntity<ByteArrayResource> fileEntity = new HttpEntity<>(new ByteArrayResource(audioBytes) {
//             @Override
//             public String getFilename() {
//                 return "audio.wav";
//             }
//         }, fileHeaders);

//         MultiValueMap<String, Object> body = new LinkedMultiValueMap<>();
//         body.add("file", fileEntity);
//         body.add("model", "whisper-1");

//         HttpEntity<MultiValueMap<String, Object>> request = new HttpEntity<>(body, headers);
//         ResponseEntity<String> response = restTemplate.postForEntity(apiURL, request, String.class);

//         if (response.getStatusCode() == HttpStatus.OK) {
//             return extractTranscript(response.getBody());
//         } else {
//             throw new RuntimeException("Whisper API Error: " + response.getBody());
//         }
//     }

//     private String extractTranscript(String jsonResponse) throws Exception {
//         JsonNode root = objectMapper.readTree(jsonResponse);
//         return root.get("text").asText();
//     }

//     public String queryGPT(String prompt) throws Exception {
//         String apiURL = "https://api.openai.com/v1/chat/completions";

//         HttpHeaders headers = new HttpHeaders();
//         headers.setContentType(MediaType.APPLICATION_JSON);
//         headers.setBearerAuth("YOUR_OPENAI_API_KEY");

//         Map<String, Object> requestBody = Map.of(
//             "model", "gpt-3.5-turbo",
//             "messages", List.of(Map.of("role", "user", "content", prompt))
//         );

//         HttpEntity<Map<String, Object>> request = new HttpEntity<>(requestBody, headers);
//         ResponseEntity<String> response = restTemplate.postForEntity(apiURL, request, String.class);

//         if (response.getStatusCode() == HttpStatus.OK) {
//             JsonNode root = objectMapper.readTree(response.getBody());
//             JsonNode choices = root.get("choices");
//             String aiReply = choices.get(0).get("message").get("content").asText();
//             return aiReply.trim();
//         } else {
//             throw new RuntimeException("GPT API Error: " + response.getBody());
//         }
//     }
// }
 
 
 
package com.internship.esp32_chatbot.service;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.core.io.ByteArrayResource;
import org.springframework.http.*;
import org.springframework.stereotype.Service;
import org.springframework.util.LinkedMultiValueMap;
import org.springframework.util.MultiValueMap;
import org.springframework.web.client.RestTemplate;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.List;
import java.util.Map;

@Service
public class AudioProcessingService {

    @Value("${ai.api-key}")
    private String aiApiKey; 
     
    @Value("${openai.api-key}")
    private String openaiApiKey; 
     


    private final LogService logService;
    private final RestTemplate restTemplate;
    private final ObjectMapper objectMapper;  
      
    //coonvert raw audio to wav format
    private byte[] wrapPCMToWav(byte[] pcmData, int sampleRate, int channels, int bitsPerSample) throws IOException {
        ByteArrayOutputStream out = new ByteArrayOutputStream();

        int byteRate = sampleRate * channels * bitsPerSample / 8;
        int blockAlign = channels * bitsPerSample / 8;
        int subchunk2Size = pcmData.length;
        int chunkSize = 36 + subchunk2Size;

        // RIFF header
        out.write("RIFF".getBytes());
        out.write(intToLittleEndian(chunkSize));
        out.write("WAVE".getBytes());

        // fmt subchunk
        out.write("fmt ".getBytes());
        out.write(intToLittleEndian(16)); // PCM header size
        out.write(shortToLittleEndian((short) 1)); // PCM format
        out.write(shortToLittleEndian((short) channels));
        out.write(intToLittleEndian(sampleRate));
        out.write(intToLittleEndian(byteRate));
        out.write(shortToLittleEndian((short) blockAlign));
        out.write(shortToLittleEndian((short) bitsPerSample));

        // data subchunk
        out.write("data".getBytes());
        out.write(intToLittleEndian(subchunk2Size));
        out.write(pcmData);

        return out.toByteArray();
    }

    private byte[] intToLittleEndian(int value) {
        return new byte[] {
            (byte) (value & 0xFF),
            (byte) ((value >> 8) & 0xFF),
            (byte) ((value >> 16) & 0xFF),
            (byte) ((value >> 24) & 0xFF)
        };
    }

    private byte[] shortToLittleEndian(short value) {
        return new byte[] {
            (byte) (value & 0xFF),
            (byte) ((value >> 8) & 0xFF)
        };
    }

    

    public AudioProcessingService(LogService logService) {
        this.logService = logService;
        this.restTemplate = new RestTemplate();
        this.objectMapper = new ObjectMapper();
    }

    public String processAudioFromMQTT(byte[] audioBytes, String deviceId) throws Exception {
        String transcript = transcribeAudio(audioBytes); 
         
        byte[] wavBytes = wrapPCMToWav(audioBytes, 16000, 1, 16);//conv to wav
        logService.saveLog(deviceId, wavBytes, transcript);
        return queryGPT(transcript);
    }

    private String transcribeAudio(byte[] audioBytes) throws Exception {
        // Step 1: Upload audio to AssemblyAI 
        byte[] wavBytes = wrapPCMToWav(audioBytes, 16000, 1, 16);//conv to wav
        String uploadUrl = uploadAudioToAssembly(wavBytes);

        // Step 2: Request transcription
        String transcriptId = requestTranscription(uploadUrl);

        // Step 3: Poll for transcript result
        return pollTranscriptResult(transcriptId);
    }

    private String uploadAudioToAssembly(byte[] audioBytes) throws Exception {
        String uploadUrl = "https://api.assemblyai.com/v2/upload";

        HttpHeaders headers = new HttpHeaders();
        headers.setContentType(MediaType.APPLICATION_OCTET_STREAM);
        headers.set("Authorization", aiApiKey); //headers.setBearerAuth(aiApiKey); 
        System.out.println("Using AssemblyAI API Key: " + aiApiKey);

        HttpEntity<ByteArrayResource> request = new HttpEntity<>(
            new ByteArrayResource(audioBytes) {
                @Override
                public String getFilename() {
                    return "audio.wav";
                }
            },
            headers
        );

        ResponseEntity<String> response = restTemplate.postForEntity(uploadUrl, request, String.class);

        if (response.getStatusCode() == HttpStatus.OK) {
            JsonNode json = objectMapper.readTree(response.getBody());
            return json.get("upload_url").asText();
        } else {
            throw new RuntimeException("Upload failed: " + response.getBody());
        }
    }

    private String requestTranscription(String audioUrl) throws Exception {
        String apiUrl = "https://api.assemblyai.com/v2/transcript";

        HttpHeaders headers = new HttpHeaders();
        headers.setContentType(MediaType.APPLICATION_JSON);
        headers.set("Authorization", aiApiKey); //headers.setBearerAuth(aiApiKey); 

        Map<String, String> body = Map.of("audio_url", audioUrl);

        HttpEntity<Map<String, String>> request = new HttpEntity<>(body, headers);
        ResponseEntity<String> response = restTemplate.postForEntity(apiUrl, request, String.class);

        if (response.getStatusCode() == HttpStatus.OK) {
            JsonNode json = objectMapper.readTree(response.getBody());
            return json.get("id").asText();
        } else {
            throw new RuntimeException("Transcription request failed: " + response.getBody());
        }
    }

    private String pollTranscriptResult(String transcriptId) throws Exception {
        String pollingUrl = "https://api.assemblyai.com/v2/transcript/" + transcriptId;

        HttpHeaders headers = new HttpHeaders();
        headers.set("Authorization", aiApiKey); //headers.setBearerAuth(aiApiKey); 

        // Simple polling loop (not efficient, just for demo)
        while (true) {
            HttpEntity<Void> request = new HttpEntity<>(headers);
            ResponseEntity<String> response = restTemplate.exchange(pollingUrl, HttpMethod.GET, request, String.class);

            if (response.getStatusCode() == HttpStatus.OK) {
                JsonNode json = objectMapper.readTree(response.getBody());
                String status = json.get("status").asText();

                if ("completed".equals(status)) {
                    return json.get("text").asText();
                } else if ("error".equals(status)) {
                    throw new RuntimeException("Transcription error: " + json.get("error").asText());
                }

                Thread.sleep(1500); // wait before next poll
            } else {
                throw new RuntimeException("Polling failed: " + response.getBody());
            }
        }
    }

    public String queryGPT(String prompt) throws Exception {
        String apiURL = "https://api.cohere.ai/v1/chat";
 
        HttpHeaders headers = new HttpHeaders();
        headers.setContentType(MediaType.APPLICATION_JSON);
        headers.set("Authorization", "Bearer " + openaiApiKey);

        Map<String, Object> requestBody = Map.of(
            "model", "command-xlarge-nightly",  // or another available model
            "message", prompt,
            "max_tokens", 100,
            "temperature", 0.7
        );

        HttpEntity<Map<String, Object>> request = new HttpEntity<>(requestBody, headers);
        ResponseEntity<String> response = restTemplate.postForEntity(apiURL, request, String.class);

        if (response.getStatusCode() == HttpStatus.OK) { 
            System.out.println("AssemblyAI raw response: " + response.toString()); 

            JsonNode root = objectMapper.readTree(response.getBody());
            return root.path("text").asText(null);
        } else {
            throw new RuntimeException("GPT API Error: " + response.getBody());
        }
    }
}
