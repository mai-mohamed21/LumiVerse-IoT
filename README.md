# 🌈 LumiVerse

---

## 🌟 **Overview**

LumiVerse is built for all children – especially those with autism or communication challenges – who sometimes struggle to express or understand emotions.  

Our system gives them a supportive companion that can recognize and visualize emotions. Inspired by Inside Out, LumiVerse brings characters to life, helping both children and their families connect better.  

Through IoT, cloud services, and AI-powered emotion analysis, LumiVerse is not just a monitoring tool – it’s a bridge for empathy and understanding. 💜  

---

## ⚙️ **Core Features**

### **ESP32 Microcontroller**
- Collects real-time sensor data.  

### **Sensors**
- Vibration Sensor  
- Temperature Sensor  
- LDR (Light Sensor)  

### **Smart Emotion System**
- Maps emotions to characters (Inside Out style).  
- Publishes live data to cloud.  
- Supports chatbot querying: “How am I feeling now?”.  

### **Database (Supabase)**
- **Tables:** users, sensor_readings, alerts, access_logs, daily_emotion_percentages.  
- **Authentication:** Supabase Auth (Login / Signup).  
- **RLS Policies:** secure data access.  
- **Triggers:** auto-updates & daily aggregation.  

### **Cloud Communication**
- **HiveMQ with MQTT topics:**  
  - `esp32/character/emotion`  
  - `flutter/sound`  

### **Applications**
- **Flutter App**  
  - Login / Signup (Supabase Auth).  
  - Dashboard with real-time sensor & emotion data.  
  - Alerts + History.  
- **Flask API** → Middleware for querying last detected emotion.  
- **FastAPI** → Extended APIs for chatbot + integrations.  

---

## 🚀 **Tech Stack**
- **Hardware:** ESP32, Sensors, Mini Player, LCD ,Keypad
- **Communication:** MQTT (HiveMQ)  
- **Backend:** Supabase (DB + Auth + API), Flask, FastAPI  
- **Frontend:** Flutter Mobile App  

---

## 🎬 **Inspiration**
Just like Inside Out gave us characters for emotions, LumiVerse gives children a real-world tool to express themselves.  

It’s not just about IoT sensors – it’s about making emotions visible, audible, and shareable.  

---
