### Modern C++: lambda, std::bind, std::function
# Лабораторная работа
## Задание к выполнению лабораторной работы.
### 1. Реализовать класс пользователя Twitter `Account` со следующим интерфейсом
```cpp
class Account
{
public:
   explicit Account(int i_id);
   void Tweet(const std::string& i_message);
   void ReTweet(int i_id, const std::string& i_message);
   void AddFollower(const std::function<void(int, const std::string&)>& i_callback);
   void SetAutoReTweet(Account& io_leader);
   const std::vector<std::string>& GetAllTweets() const;

private:
   const int m_id;
   std::vector<std::string> m_tweets; // содержит твиты и ретвиты
   std::vector<std::function<void(int, const std::string&)>> m_followers;
};
```
#### Описание функций
Конструктор. Создание пользователя с номером `i_id`.
```cpp
   explicit Account(int i_id);
```
Функция создания нового твита.
Должна добавлять новый твит `i_message` в `m_tweets` и уведомлять всех подписчиков `m_followers`.
```cpp
   void Tweet(const std::string& i_message);
```
Функция репоста чужого твита к себе на страницу.
`i_id` - номер пользователя, который сделал твит.
`i_message` - текст твита.
Твит добавляется в `m_tweets` в формате `i_id + ":" + i_message`. В этой функции уведомлять подписчиков не нужно, иначе может получиться циклическая зависимость.
```cpp
   void ReTweet(int i_id, const std::string& i_message);
```
Функция добавления подписки на этот аккаунт.
`i_callback` - callback, который необходимо вызвать при создании твита от этого аккаунта и передать id этого пользователя, и текст твита.
Добавление `i_callback` происходит в `m_followers`.
```cpp
   void AddFollower(const std::function<void(int, const std::string&)>& i_callback);
```
Функция настройки автоматического ретвита всех твитов `io_leader`.
Реализовать с помощью добавления в подписки `io_leader` лямбды или std::bind.
```cpp
   void SetAutoReTweet(Account& io_leader);
```
Функция получения всех твитов на странице пользователя.
```cpp
   const std::vector<std::string>& GetAllTweets() const;
```

### 2. Проверить работу созданного класса
1. Создать вектор пользователей;
2. Настроить автоматический ретвит некоторых пользователей другими пользователями;
3. Добавить подписку некоторым пользователям так, чтобы их твит выводился в консоль (добавить в подписки к ним лямбду, которая это делает);
4. Сделать твиты от некоторых пользователей;
5. Проверить, что при твите одного пользователя, все подписанные пользователи делают ретвиты;
6. Проверить, что появляются сообщения в консоли о новых твитах.
