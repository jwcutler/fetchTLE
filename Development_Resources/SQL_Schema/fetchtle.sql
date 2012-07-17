/*
fetchTLE SQL schema definition file.
*/

CREATE TABLE admins (
    id int AUTO_INCREMENT,
    username VARCHAR(50),
    password VARCHAR(50),
    PRIMARY KEY(`id`)
);

CREATE TABLE sources (
    id int AUTO_INCREMENT,
    name varchar(50) NOT NULL,
    url varchar(255) NOT NULL,
    description text,
    created_on datetime NOT NULL,
    updated_on datetime NOT NULL,
    last_polled datetime,
    PRIMARY KEY(`id`)
);

CREATE TABLE tles (
    id int AUTO_INCREMENT,
    source_id int NOT NULL,
    name varchar(50) NOT NULL,
    satellite_number int NOT NULL,
    classification char(1) NOT NULL,
    launch_year int NOT NULL,
    launch_number int NOT NULL,
    launch_piece varchar(3) NOT NULL,
    epoch_year int NOT NULL,
    epoch varchar(50) NOT NULL,
    ftd_mm_d2 varchar(50) NOT NULL,
    std_mm_d6 varchar(50) NOT NULL,
    bstar_drag varchar(50) NOT NULL,
    element_number int NOT NULL,
    checksum_l1 int NOT NULL,
    inclination varchar(50) NOT NULL,
    right_ascension varchar(50) NOT NULL,
    eccentricity varchar(50) NOT NULL,
    perigee varchar(50) NOT NULL,
    mean_anomaly varchar(50) NOT NULL,
    mean_motion varchar(50) NOT NULL,
    revs int NOT NULL,
    checksum_l2 int NOT NULL,
    raw_l1 text NOT NULL,
    raw_l2 text NOT NULL,
    created_on datetime NOT NULL,
    PRIMARY KEY(`id`)
);

/* Load in the default values. */
INSERT INTO `admins` (`id`, `username`, `password`) VALUES (1, 'admin', 'dummypassword');
