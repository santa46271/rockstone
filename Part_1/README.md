# Часть 1: Код-ревью

## Проблемные участки
1. Обработка событий (архитектура)  
   Switch-case в `on_event` плохо масштабируется. Лучше использовать таблицу обработчиков.

2. Обработка пакетов (архитектура)  
   Аналогичная проблема в `on_packet`. Лучше заменить на `std::unordered_map<PacketType, std::function>`.

3. Асинхронный колбэк (Потенциальное UB)  
   Локальный объект `LoginData` в методе `login` передается в асинхронный колбэк, что приводит к UB.

Реализованное исправление
Исправлена проблема №3:
- Заменен локальный объект на `std::shared_ptr<LoginData>`
- Лямбда захватывает умный указатель по значению
- Исправлена сигнатура метода (несоответствие типа пакета)

## Патч
--- Client.cpp	2025-07-19 00:52:44.595294295 +0300
+++ Part_1/Client.cpp	2025-07-23 00:59:20.671076517 +0300
@@ -61,7 +61,7 @@
 
 void Client::params_set(const server::Packet &packet) const
 {
-	string params = packet.S(0);
+    const string& params = packet.S(0);
 	if (params.empty())
 		return;
 
@@ -86,11 +86,11 @@
 	logger->info("Client {} bought item {}", this->player->id, item_id);
 }
 
-void Client::login(const client::Packet *packet)
+void Client::login(const server::Packet &packet)
 {
-	uint64_t net_id = packet->L(0);
-	uint8_t net_type = packet->B(1);
-	const string &auth_key = packet->S(3);
+	uint64_t net_id = packet.L(0);
+	uint8_t net_type = packet.B(1);
+	const string &auth_key = packet.S(3);
 
 	if (net_type >= NetType::MAX_TYPE)
 	{
@@ -106,12 +106,12 @@
 		return;
 	}
 
-	LoginData data;
-	data.net_id = net_id;
-	data.net_type = net_type;
+    auto data = std::make_shared<LoginData>();
+	data->net_id = net_id;
+	data->net_type = net_type;
 
-	this->requests->add(&data, [&](const vector<Player*> &loaded) -> void
+	this->requests->add(data.get(), [this, data](const vector<Player*> &loaded) -> void
 	{
-		this->login_do(loaded[0], &data);
+		this->login_do(loaded[0], data.get());
 	});
-}
\ No newline at end of file
+}
