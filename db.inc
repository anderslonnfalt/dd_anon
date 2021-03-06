<?php

class db {
	private static $db = null;

	private static function create_connection (){
		$data_source = "mysql:host=" . DB_ANON_HOST . ";dbname=" . DB_ANON_NAME;
		$conn = new PDO($data_source, DB_ANON_USERNAME, DB_ANON_PASSWORD);
		$conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
		$conn->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
		return $conn;
	}

	private static function connect (){
		if (self::$db == null) 
			self::$db = self::create_connection();  
	}

	private static function create_statement ($query){
		self::connect();
		$stmt = self::$db->prepare($query);
		return $stmt;
	}

	private static function execute ($query, $values){
		$stmt = self::create_statement($query);
		$stmt->execute($values);
		return $stmt;
	}

	// returns a result array from a select query
	public static function select ($query, $values){
		$stmt = self::execute($query, $values);
		return $stmt->fetchAll();
	}

	// returns single row from a select query
	public static function select_single_row ($query, $values){
		$stmt = self::execute($query, $values);
		$result = $stmt->fetchAll();
		return $result[0];
	}

	// returns single value from a select query
	public static function select_single_value ($query, $values){
		$stmt = self::execute($query, $values);
		$result = $stmt->fetchAll();
		if (count($result) == 0) return null;
		$row = $result[0];
		return $row[0];
	}

	// executes an insert query, returns insert_id
	public static function insert ($query, $values){
		$stmt = self::execute($query, $values);
		return self::$db->lastInsertId();
	}

	// executes an update query, returns number of affected rows
	public static function update ($query, $values){
		$stmt = self::execute($query, $values);
		return $stmt->rowCount();
	}

	// executes multiple similar delete queries, takes id values from array, returns number of affected rows
	public static function update_from_array ($query, $id_array){
		$stmt = self::create_statement($query);
		foreach($id_array as $id_value){
			$values = Array($id_value);
			$stmt->execute($values);
		}
		return count($id_array);
	}

	// executes a delete query, returns number of affected rows
	public static function delete ($query, $values){
		$stmt = self::execute($query, $values);
		return $stmt->rowCount();
	}

	// inserts multiple rows from single query, uses 1 fixed variable and 1 array variable that changes for each row, returns number of affected rows
	public static function insert_multiple_rows_with_two_variables ($query, $variable_array, $second_variable){
		$stmt = self::create_statement($query);
		$counter = 0;
		foreach($variable_array as $first_variable){
			$values = Array($first_variable, $second_variable);
			$stmt->execute($values);
			$counter = $counter + $stmt->rowCount();
		}
		return $counter;
	}

	public static function insert_ignore_from_array ($query, $variable_array, $second_variable){
		$stmt = self::create_statement($query);
		$counter = 0;
		foreach($variable_array as $first_variable){
			$values = Array($first_variable, $second_variable, $first_variable, $second_variable);
			$stmt->execute($values);
			$counter = $counter + $stmt->rowCount();
		}
		return $counter;
	}

	// inserts new row if it does not already exists
	public static function insert_ignore ($db_table, $columns, $values){
		$select_statement = "";
		foreach ($columns as $value)
			$select_statement = $select_statement . " and " . $value . " = ?";
		$select_statement = trim($select_statement, " and");
		$query = "select id from " . $db_table . " where " . $select_statement;
		$if_exists = self::select_single_value($query, $values);
		if(empty($if_exists)){
			$question_marks = "";
			foreach ($columns as $value)
				$question_marks = $question_marks . ", ?";
			$question_marks = trim($question_marks, ",");
			$query = "insert into " . $db_table . "(" . implode(",", $columns) . ") values(" . $question_marks . ")";
			return self::insert($query, $values);
		}
		return false;
	}

	public static function insert_all ($query, $insert_array){
		$stmt = self::create_statement();
		$counter = 0;
		foreach($insert_array as $row){
			$values = Array($row[0]);
			$stmt->execute($values);
			$counter = $counter + $stmt->rowCount();
		}
		return $counter;
	}
}
?>