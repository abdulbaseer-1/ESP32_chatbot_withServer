package com.internship.esp32_chatbot.repository;

import com.internship.esp32_chatbot.model.AudioLog;
import org.springframework.data.jpa.repository.JpaRepository;

public interface AudioLogRepository extends JpaRepository<AudioLog, Long> {
}