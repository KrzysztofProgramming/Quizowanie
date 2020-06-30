PRAGMA foreign_keys = ON;

DROP TABLE IF EXISTS quiz;

CREATE TABLE quiz (
    quizId INTEGER PRIMARY KEY AUTOINCREMENT,
    quizTitle VARCHAR(50) UNIQUE NOT NULL,
    maxTime TIME,
    imgPath VARCHAR(250)
);
CREATE INDEX idx_quizTitle ON quiz (quizTitle);

DROP TABLE IF EXISTS questions;

CREATE TABLE questions(
    questionId INTEGER PRIMARY KEY AUTOINCREMENT,
    quizId INT NOT NULL,
    questionDescription VARCHAR(1000) NOT NULL,
        isMulti BIT NOT NULL,
        points INT NOT NULL,
    maxTime TIME,
    imgPath VARCHAR(500),
    CONSTRAINT `fk_quizId` FOREIGN KEY (quizId) REFERENCES quiz(quizId) ON UPDATE CASCADE ON DELETE CASCADE
);
CREATE INDEX idx_quizId ON questions (quizId);

DROP TABLE IF EXISTS answers;

CREATE TABLE answers(
    questionId INT NOT NULL,
    answerNumber INT NOT NULL,
    answerDescription VARCHAR(100) NOT NULL,
    isGood BIT NOT NULL,
    PRIMARY KEY(questionId, answerNumber),
    CONSTRAINT `fk_questionId` FOREIGN KEY (questionId) REFERENCES questions(questionId) ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE INDEX idx_questionId ON answers (questionId);
