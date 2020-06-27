DROP TABLE IF EXISTS quiz;
CREATE TABLE quiz (
quizId INT,
quizTitle VARCHAR(50) UNIQUE NOT NULL,
maxTime INT,
PRIMARY KEY(quizId)
);
DROP TABLE IF EXISTS questions;
CREATE TABLE questions(
questionId INT,
quizId INT NOT NULL,
questionDescription VARCHAR(1000) NOT NULL,
maxTime INT,
PRIMARY KEY(questionId),
CONSTRAINT `fk_quizId` FOREIGN KEY (quizId) REFERENCES quiz(quizId) ON UPDATE CASCADE ON DELETE RESTRICT
);
CREATE INDEX idx_quizId ON questions (quizId);
DROP TABLE IF EXISTS answers;
CREATE TABLE answers(
questionId INT,
answerDescription VARCHAR(100) NOT NULL,
answerNumber INT,
PRIMARY KEY(questionId, answerNumber),
CONSTRAINT `fk_questionId` FOREIGN KEY (questionId) REFERENCES questions(questionId) ON UPDATE CASCADE ON DELETE RESTRICT
);
CREATE INDEX idx_questionId ON answers (questionId);
