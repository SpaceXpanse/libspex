)";

} // anonymous namespace

void
InternalSetupGameChannelsSchema (SQLiteDatabase& db)
{
  LOG (INFO) << "Setting up the database schema for side channels...";
  db.Execute (SCHEMA_SQL);
}

} // namespace spacexpanse
