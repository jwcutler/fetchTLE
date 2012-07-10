CREATE DATABASE IF NOT EXISTS fetchTLE;
GRANT ALL PRIVILEGES ON fetchTLE.* TO fetchTLE@"localhost";
GRANT ALL PRIVILEGES ON fetchTLE.* TO fetchTLE@"localhost.localdomain";

USE fetchTLE;

DROP TABLE IF EXISTS keps;

CREATE TABLE keps (
        _key INT AUTO_INCREMENT,			-- Unique key for the record
        common_name VARCHAR (255) NOT NULL,		-- Common name of the Satellite
        satID VARCHAR (255) NOT NULL,			-- Satellite ID
	insertion_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
	epoch_time TIMESTAMP,                            -- Epoch time stamp from Kep, line 1.
        line_one VARCHAR (255) NOT NULL,		-- Line one of the keps
        line_two VARCHAR (255) NOT NULL,		-- Line two of the keps
        PRIMARY KEY (_key)
) TYPE = InnoDB;


