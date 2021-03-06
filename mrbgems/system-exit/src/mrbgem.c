#include <mruby.h>
#include <mruby/string.h>
#include <mruby/variable.h>

#include <mruby/system_exit/abort.h>
#include <mruby/system_exit/system_exit.h>

void mrb_noreturn
mrb_abort(mrb_state* mrb, mrb_value message) {
  mrb_value system_exit;

  if(mrb_string_p(message)) {
    fprintf(stderr, "%s\n", RSTRING_CSTR(mrb, message));
  } else {
    message = mrb_str_new_cstr(mrb, "SystemExit");
  }

  system_exit = mrb_funcall(mrb, mrb_obj_value(E_SYSTEM_EXIT), "new", 2, mrb_fixnum_value(1), message);

  mrb_exc_raise(mrb, system_exit);
}

static mrb_value
mrb_f_abort(mrb_state* mrb, mrb_value self) {
  mrb_value message;

  mrb_get_args(mrb, "|S", &message);

  mrb_abort(mrb, message);
}

static mrb_value
mrb_system_exit_status(mrb_state* mrb, mrb_value self) {
  return mrb_attr_get(mrb, self, mrb_intern_lit(mrb, "status"));
}

static mrb_value
mrb_system_exit_initialize(mrb_state* mrb, mrb_value system_exit) {
  mrb_int status;
  mrb_value message;

  mrb_int argc;

  argc = mrb_get_args(mrb, "|iS", &status, &message);

  if(argc == 0) {
    status = 0;
  }
  mrb_iv_set(mrb, system_exit, mrb_intern_lit(mrb, "status"), mrb_fixnum_value(status));

  if(argc == 2) {
    mrb_iv_set(mrb, system_exit, mrb_intern_lit(mrb, "mesg"), message);
  } else {
    message = mrb_str_new_cstr(mrb, "(no message)");
  }

  return system_exit;
}

void
mrb_mruby_system_exit_gem_init(mrb_state* mrb) {
  struct RClass* system_exit;

  system_exit = mrb_define_class(mrb, "SystemExit", mrb->eException_class);
  mrb_define_method(mrb, system_exit, "initialize", mrb_system_exit_initialize, MRB_ARGS_OPT(2));
  mrb_define_method(mrb, system_exit, "status", mrb_system_exit_status, MRB_ARGS_NONE());

  mrb_define_method(mrb, mrb->kernel_module, "abort", mrb_f_abort, MRB_ARGS_OPT(1));

  return;
}

void
mrb_mruby_system_exit_gem_final(mrb_state* mrb) {
  return;
}
