# Часть 3: SQL-запросы

## 1. 5 самых активных устройств
SELECT device, COUNT(*) AS login_count
FROM players
GROUP BY device
ORDER BY login_count DESC
LIMIT 5;

## Среднее число логинов в день (за 7 дней)
SELECT COUNT(*) / 7.0 AS avg_logins_per_day
FROM players
WHERE login_time >= CURDATE() - INTERVAL 6 DAY;
