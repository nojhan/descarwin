
-- Descarwin tables creation script
--
-- tested with MySQL 14.14 Distrib 5.1.61, for redhat-linux-gnu (x86_64) using readline 5.1
-- 
-- Not anymore tested with PostgreSQL 8.1.22 (on a Red Hat Entreprise Linux: 8.1.22-1.el5_5.1)
--   install postgresql-server and postgresql
--   use the postgres user: su - postgres
--   create a database: createdb test
--   import the script: psql -d test -a -f database_structure.sql
-- 
-- NOTE: with PostgreSQL, the PRIMARY KEY NOT NULL auto_increment statement create an implicit index called "<table name>_pkey"
--       (e.g. algorithm_pkey), this is a feature.

-- ------------------------
-- TERMINAL DATA TABLES --
-- ------------------------

-- Algorithms versions
-- CREATE SEQUENCE algorithm_id_seq;
CREATE TABLE algorithm (
    id INT4 PRIMARY KEY NOT NULL auto_increment,
-- id INT4 DEFAULT nextval('algorithm_id_seq') PRIMARY KEY NOT NULL auto_increment,
    svn_tag VARCHAR(64) NOT NULL DEFAULT '',
    date TIMESTAMP NOT NULL DEFAULT NOW(),
    description TEXT NOT NULL DEFAULT '',
    shortname VARCHAR(64) NOT NULL DEFAULT '',
    source_link VARCHAR(128) NOT NULL DEFAULT '',
    CONSTRAINT svn_tag_constraint UNIQUE (svn_tag)
);

-- Domains
-- CREATE SEQUENCE domain_id_seq;
CREATE TABLE domain (
    id INT4 PRIMARY KEY NOT NULL auto_increment,
-- id INT4 DEFAULT nextval('domain_id_seq') PRIMARY KEY NOT NULL auto_increment,
    name VARCHAR(64) NOT NULL DEFAULT '',
    IPC INT4 NOT NULL DEFAULT 0,
    version VARCHAR(64) NOT NULL DEFAULT '',
    temporal BOOLEAN DEFAULT FALSE
);

-- Hardware
-- CREATE SEQUENCE hardware_id_seq;
CREATE TABLE hardware (
    id INT4 PRIMARY KEY NOT NULL auto_increment,
-- id INT4 DEFAULT nextval('hardware_id_seq') PRIMARY KEY NOT NULL auto_increment,
    archi VARCHAR(64) NOT NULL DEFAULT '',
    CPU_frequency INT4 NOT NULL DEFAULT 0,
    RAM INT4 NOT NULL DEFAULT 0,
    cache_size INT4 NOT NULL DEFAULT 0,
    OS VARCHAR(128) NOT NULL DEFAULT ''
);

-- Parameters
-- CREATE SEQUENCE parameter_set_id_seq;
CREATE TABLE parameter_set (
    id INT4 PRIMARY KEY NOT NULL auto_increment,
-- id INT4 DEFAULT nextval('parameter_set_id_seq') PRIMARY KEY NOT NULL auto_increment,
    id_set INT4 DEFAULT 0, -- the parameter belongs to this set
    param_name VARCHAR(64) NOT NULL DEFAULT '',
    param_value VARCHAR(64) NOT NULL DEFAULT '',
    CONSTRAINT constraint_set_name UNIQUE (id_set,param_name) -- their should be only one param per set
);

-- Solutions
-- CREATE SEQUENCE solution_id_seq;
CREATE TABLE solution (
    id INT4 PRIMARY KEY NOT NULL auto_increment,
-- id INT4 DEFAULT nextval('solution_id_seq') PRIMARY KEY NOT NULL auto_increment,
    plan TEXT NOT NULL DEFAULT '',
    decomposition TEXT NOT NULL DEFAULT '',
    expanded_nodes INT4 NOT NULL DEFAULT 0,
    at_iteration INT4 NOT NULL DEFAULT 0,
    at_evaluation INT4 NOT NULL DEFAULT 0,
    is_best_in_run BOOLEAN DEFAULT FALSE,
    is_best_in_iteration BOOLEAN DEFAULT FALSE,
    is_best_in_instance BOOLEAN DEFAULT FALSE
);



-- ----------------------------
-- INTERMEDIATE DATA TABLES --
-- ----------------------------

-- Instances
-- CREATE SEQUENCE instance_id_seq;
CREATE TABLE instance (
    id INT4 PRIMARY KEY NOT NULL auto_increment,
-- id INT4 DEFAULT nextval('instance_id_seq') PRIMARY KEY NOT NULL auto_increment,
    id_domain INT4 NOT NULL DEFAULT 0 REFERENCES domain(id),
    number INT4 NOT NULL DEFAULT 0,
    -- not in the Domain table because could be the same as the instance, as in IPC6 and some IPC3
    file_domain VARCHAR(128) NOT NULL DEFAULT '', 
    file_instance VARCHAR(128) NOT NULL DEFAULT '',
    id_optimum_solution INT4 NOT NULL DEFAULT 0 REFERENCES solution(id)
);
CREATE INDEX idx_instance_id_domain ON instance(id_domain);


-- Campaigns: a specific algorithm version with a specific tuning
-- CREATE SEQUENCE campaign_id_seq;
CREATE TABLE campaign (
    id INT4 PRIMARY KEY NOT NULL auto_increment,
-- id INT4 DEFAULT nextval('campaign_id_seq') PRIMARY KEY NOT NULL auto_increment,
    id_algo INT4 NOT NULL DEFAULT 0 REFERENCES algorithm(id),
    date TIMESTAMP NOT NULL DEFAULT NOW(),
    tuning_method VARCHAR(128) NOT NULL DEFAULT '', 
    tuning_version VARCHAR(128) NOT NULL DEFAULT '',
    description TEXT NOT NULL DEFAULT ''
);
CREATE INDEX idx_campaign_id_algo ON campaign(id_algo);


-- Runs
-- CREATE SEQUENCE run_id_seq;
CREATE TABLE run (
    id INT4 PRIMARY KEY NOT NULL auto_increment,
-- id INT4 DEFAULT nextval('run_id_seq') PRIMARY KEY NOT NULL auto_increment,
    id_hardware INT4 NOT NULL DEFAULT 0 REFERENCES hardware(id),
    id_instance INT4 NOT NULL DEFAULT 0 REFERENCES instance(id),
    id_param INT4 NOT NULL DEFAULT 0 REFERENCES instance(id),
    date_start TIMESTAMP NOT NULL DEFAULT NOW(),
    date_end TIMESTAMP NOT NULL DEFAULT 0,
    CONSTRAINT constraint_runs_dates_differents CHECK (date_start != date_end)
);
CREATE INDEX idx_run_id_hardware ON run(id_hardware);
CREATE INDEX idx_run_id_instance ON run(id_instance);
CREATE INDEX idx_run_id_param ON run(id_param);

-- ----------------
-- LINKS TABLES --
-- ----------------

-- campaign <-> run
-- CREATE SEQUENCE campaign_run_id_seq;
CREATE TABLE campaign_run (
    id INT4 PRIMARY KEY NOT NULL auto_increment,
-- id INT4 DEFAULT nextval('campaign_run_id_seq') PRIMARY KEY NOT NULL auto_increment,
    id_campaign INT4 NOT NULL DEFAULT 0 REFERENCES campaign(id),
    id_run INT4 NOT NULL DEFAULT 0 REFERENCES run(id)
    -- at_iteration INT4 NOT NULL DEFAULT 0 -- FIXME what was it for?
);

-- solution <-> run
-- CREATE SEQUENCE solution_run_id_seq;
CREATE TABLE solution_run (
    id INT4 PRIMARY KEY NOT NULL auto_increment,
-- id INT4 DEFAULT nextval('solution_run_id_seq') PRIMARY KEY NOT NULL auto_increment,
    id_solution INT4 NOT NULL DEFAULT 0 REFERENCES solution(id),
    id_run INT4 NOT NULL DEFAULT 0 REFERENCES run(id)
);

-- instance <-> solution
-- CREATE SEQUENCE solution_instance_id_seq;
CREATE TABLE solution_instance (
    id INT4 PRIMARY KEY NOT NULL auto_increment,
-- id INT4 DEFAULT nextval('solution_instance_id_seq') PRIMARY KEY NOT NULL auto_increment,
    id_instance INT4 NOT NULL DEFAULT 0 REFERENCES instance(id),
    id_solution INT4 NOT NULL DEFAULT 0 REFERENCES solution(id),
    description TEXT NOT NULL DEFAULT ''
);

