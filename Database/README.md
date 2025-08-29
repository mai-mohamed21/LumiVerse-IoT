# 📂 LumiVerse Database

This folder contains all database-related files for the **LumiVerse-IoT** project.  
The database is hosted on **Supabase (PostgreSQL)** and is responsible for storing logs, alerts, and policies used across the LumiVerse system.

---

## 🗄️ Files in this folder

- **`schema.sql`** → Defines the database schema (tables, relationships, constraints).
- **`policies.json`** → Contains Supabase RLS (Row-Level Security) policies for controlling access.
- **`Supabase Snippet Trigger Information Retrieval.csv`** → Example of trigger-related data exports.
- **`supabase-schema-....png`** → Schema diagram (visual representation).

---

## 🏗️ Database Structure

Main tables include:

-users → stores user information.
-emotion_logs → records chatbot interactions and detected moods.
-character_emotions → links characters with user emotions.
-sensor_readings → collects IoT sensor data.
-daily_emotion_percentages → daily stats of user emotions.
-sensor_averages → average values of sensor readings.
-alerts → stores system alerts and notifications.
-sessions → tracks user login and activity sessions.

---

## 🔐 Security Policies (RLS)

The `policies.json` file defines **Row Level Security** rules.  
Examples:
- Only authenticated users can view their own logs.
- Alerts are **read-only** for regular users but **read/write** for admins.
- System triggers auto-insert events into logs (no manual tampering).

---

## ⚡ Triggers

Triggers automatically handle repetitive tasks:
- Insert an alert into `alerts` when a sensor threshold is exceeded.
- Update user activity in `access_logs` on every login/logout.
- Maintain aggregated statistics (stored in `sensor_averages` table).

---

## 🚀 How to Use

1. Run the schema to create tables:
   ```sql
   \i schema.sql
