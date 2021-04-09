
case 0:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
	if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	  fprintf(file_out_ptr, "d%d ", opt_tmp->hex);
	  i = inz;
	  return SUCCEEDED;
	}
	if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	  break;
 }
break;

case 11:
case 1:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
	if (opt_tmp->op[x] == 'x') {
	  y = i;
	  i = inz;
	  z = input_number();
	  inz = i;
	  i = y;
	  if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
	    return FAILED;
	  if (z == SUCCEEDED && (d > 127 || d < -128)) {
	    print_error("Out of 8bit range.\n", ERROR_NUM);
	    return FAILED;
	  }

	  for (x++; x < OP_SIZE_MAX; inz++, x++) {
	    if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	      if (z == SUCCEEDED)
					fprintf(file_out_ptr, "d%d d%d ", opt_tmp->hex, d);
	      else if (z == INPUT_NUMBER_ADDRESS_LABEL)
					fprintf(file_out_ptr, "k%d d%d R%s ", active_file_info_last->line_current, opt_tmp->hex, label);
	      else {
					fprintf(file_out_ptr, "d%d c%d ", opt_tmp->hex, latest_stack);
					if (opt_tmp->type == 11) {
						/* 11 -> let's configure the stack so that all label references inside are relative */
						stacks_tmp->relative_references = 1;
					}
				}

	      i = inz;
	      return SUCCEEDED;
	    }
	    if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	      break;
	  }
	}
	if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	  break;
 }
break;

case 2:
if (xbit_size > 16 && opt_tmp->skip_xbit == 1)
	break;
for ( ; x < OP_SIZE_MAX; inz++, x++) {
	if (opt_tmp->op[x] == '?') {
	  y = i;
	  i = inz;
	  z = input_number();
	  inz = i;
	  i = y;
	  if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
	    return FAILED;
	  if (operand_hint == HINT_24BIT && opt_tmp->skip_xbit == 1)
	    break;
	  if (z == SUCCEEDED && (d < -32768 || d > 65535)) {
	    if (opt_tmp->skip_xbit == 1)
	      break;
	    print_error("Out of 16bit range.\n", ERROR_NUM);
	    return FAILED;
	  }

	  for (x++; x < OP_SIZE_MAX; inz++, x++) {
	    if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	      if (z == SUCCEEDED)
					fprintf(file_out_ptr, "d%d y%d ", opt_tmp->hex, d);
	      else if (z == INPUT_NUMBER_ADDRESS_LABEL)
					fprintf(file_out_ptr, "k%d d%d r%s ", active_file_info_last->line_current, opt_tmp->hex, label);
	      else
					fprintf(file_out_ptr, "d%d C%d ", opt_tmp->hex, latest_stack);

	      i = inz;
	      return SUCCEEDED;
	    }
	    if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	      break;
	  }
	}
	if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	  break;
 }
break;

case 3:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
	if (opt_tmp->op[x] == '&') {
	  y = i;
	  i = inz;
	  z = input_number();
	  inz = i;
	  i = y;
	  if (!(z == SUCCEEDED || z == INPUT_NUMBER_STACK || z == INPUT_NUMBER_ADDRESS_LABEL))
	    return FAILED;

	  for (x++; x < OP_SIZE_MAX; inz++, x++) {
	    if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	      if (z == SUCCEEDED)
					fprintf(file_out_ptr, "d%d z%d ", opt_tmp->hex, d);
	      else if (z == INPUT_NUMBER_ADDRESS_LABEL)
					fprintf(file_out_ptr, "k%d d%d q%s ", active_file_info_last->line_current, opt_tmp->hex, label);
	      else
					fprintf(file_out_ptr, "d%d T%d ", opt_tmp->hex, latest_stack);
	      i = inz;
	      return SUCCEEDED;
	    }
	    if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	      break;
	  }
	}
	if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	  break;
 }
break;

/* IMMEDIATE VALUES: ACCUMULATOR */

case 4:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
	if (opt_tmp->op[x] == 'x') {
	  y = i;
	  i = inz;
	  z = input_number();
	  inz = i;
	  i = y;
	  if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
	    return FAILED;

	  /* select the mode */
	  if (operand_hint == HINT_8BIT)
	    y = 0;
	  else if (operand_hint == HINT_16BIT)
	    y = 1;
	  else if (accu_size == 8)
	    y = 0;
	  else
	    y = 1;

	  if (y == 0) {
	    if (z == SUCCEEDED && (d > 255 || d < -128)) {
	      print_error("Out of 8bit range.\n", ERROR_NUM);
	      return FAILED;
	    }
	  }
	  else {
	    if (z == SUCCEEDED && (d > 65535 || d < -32768)) {
	      print_error("Out of 16bit range.\n", ERROR_NUM);
	      return FAILED;
	    }
	  }

	  for (x++ ; x < OP_SIZE_MAX; inz++, x++) {
	    if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	      /* 8BIT */
	      if (y == 0) {
					if (z == SUCCEEDED)
						fprintf(file_out_ptr, "d%d d%d ", opt_tmp->hex, d);
					else if (z == INPUT_NUMBER_ADDRESS_LABEL)
						fprintf(file_out_ptr, "k%d d%d Q%s ", active_file_info_last->line_current, opt_tmp->hex, label);
					else
						fprintf(file_out_ptr, "d%d c%d ", opt_tmp->hex, latest_stack);
	      }
	      /* 16BIT */
	      else {
					if (z == SUCCEEDED)
						fprintf(file_out_ptr, "d%d y%d ", opt_tmp->hex, d);
					else if (z == INPUT_NUMBER_ADDRESS_LABEL)
						fprintf(file_out_ptr, "k%d d%d r%s ", active_file_info_last->line_current, opt_tmp->hex, label);
					else
						fprintf(file_out_ptr, "d%d C%d ", opt_tmp->hex, latest_stack);
	      }

	      i = inz;
	      return SUCCEEDED;
	    }
	    if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	      break;
	  }
	}
	if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	  break;
 }
break;

/* MVN & MVP */

case 5:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
	if (opt_tmp->op[x] == 'x') {
	  y = i;
	  i = inz;
	  z = input_number();
	  inz = i;
	  i = y;
	  if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
	    return FAILED;
	  if (z == SUCCEEDED && (d > 255 || d < -128))
	    break;

	  e = d;
	  v = z;
	  h = latest_stack;
	  if (z == INPUT_NUMBER_ADDRESS_LABEL)
	    strcpy(labelx, label);

	  for (x++; x < OP_SIZE_MAX; inz++, x++) {
	    if (opt_tmp->op[x] == 'x') {
	      y = i;
	      i = inz;
	      z = input_number();
	      inz = i;
	      i = y;
	      if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
					return FAILED;
	      if (z == SUCCEEDED && (d > 255 || d < -128))
					break;

	      for (x++; x < OP_SIZE_MAX; inz++, x++) {
					if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
						if (v == SUCCEEDED)
							fprintf(file_out_ptr, "d%d d%d ", opt_tmp->hex, e);
						else if (v == INPUT_NUMBER_ADDRESS_LABEL)
							fprintf(file_out_ptr, "k%d d%d Q%s ", active_file_info_last->line_current, opt_tmp->hex, labelx);
						else
							fprintf(file_out_ptr, "d%d c%d ", opt_tmp->hex, h);

						if (z == SUCCEEDED)
							fprintf(file_out_ptr, "d%d ", d);
						else if (z == INPUT_NUMBER_ADDRESS_LABEL)
							fprintf(file_out_ptr, "k%d Q%s ", active_file_info_last->line_current, label);
						else
							fprintf(file_out_ptr, "c%d ", latest_stack);

						i = inz;
						return SUCCEEDED;
					}
					if (opt_tmp->op[x] != toupper((int)buffer[inz]))
						break;
	      }
	    }
	    if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	      break;
	  }
	}
	if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	  break;
 }
break;

/* SEP & REP */

case 6:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
	if (opt_tmp->op[x] == 'x') {
	  y = i;
	  i = inz;
	  z = input_number();
	  inz = i;
	  i = y;
	  if (z != SUCCEEDED) {
	    if (opt_tmp->skip_xbit == 0)
	      print_error("REP needs immediate data.\n", ERROR_LOG);
	    else
	      print_error("SEP needs immediate data.\n", ERROR_LOG);
	    return FAILED;
	  }
	  if (z == SUCCEEDED && (d > 255 || d < -128))
	    break;

	  for (x++; x < OP_SIZE_MAX; inz++, x++) {
	    if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {

	      fprintf(file_out_ptr, "d%d d%d ", opt_tmp->hex, d);

	      /* REP */
	      if (opt_tmp->skip_xbit == 0) {
					if (d & 16)
						index_size = 16;
					if (d & 32)
						accu_size = 16;
	      }
	      /* SEP */
	      else {
					if (d & 16)
						index_size = 8;
					if (d & 32)
						accu_size = 8;
	      }

	      i = inz;
	      return SUCCEEDED;
	    }
	    if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	      break;
	  }
	}
	if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	  break;
 }
break;

/* IMMEDIATE VALUES: INDEX */

case 7:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
	if (opt_tmp->op[x] == 'x') {
	  y = i;
	  i = inz;
	  z = input_number();
	  inz = i;
	  i = y;
	  if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
	    return FAILED;

	  /* select the mode */
	  if (operand_hint == HINT_8BIT)
	    y = 0;
	  else if (operand_hint == HINT_16BIT)
	    y = 1;
	  else if (index_size == 8)
	    y = 0;
	  else
	    y = 1;

	  if (y == 0) {
	    if (z == SUCCEEDED && (d > 255 || d < -128)) {
	      print_error("Out of 8bit range.\n", ERROR_NUM);
	      return FAILED;
	    }
	  }
	  else {
	    if (z == SUCCEEDED && (d > 65535 || d < -32768)) {
	      print_error("Out of 16bit range.\n", ERROR_NUM);
	      return FAILED;
	    }
	  }

	  for (x++; x < OP_SIZE_MAX; inz++, x++) {
	    if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	      /* 8BIT */
	      if (y == 0) {
					if (z == SUCCEEDED)
						fprintf(file_out_ptr, "d%d d%d ", opt_tmp->hex, d);
					else if (z == INPUT_NUMBER_ADDRESS_LABEL)
						fprintf(file_out_ptr, "k%d d%d Q%s ", active_file_info_last->line_current, opt_tmp->hex, label);
					else
						fprintf(file_out_ptr, "d%d c%d ", opt_tmp->hex, latest_stack);
	      }
	      /* 16BIT */
	      else {
					if (z == SUCCEEDED)
						fprintf(file_out_ptr, "d%d y%d ", opt_tmp->hex, d);
					else if (z == INPUT_NUMBER_ADDRESS_LABEL)
						fprintf(file_out_ptr, "k%d d%d r%s ", active_file_info_last->line_current, opt_tmp->hex, label);
					else
						fprintf(file_out_ptr, "d%d C%d ", opt_tmp->hex, latest_stack);
	      }

	      i = inz;
	      return SUCCEEDED;
	    }
	    if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	      break;
	  }
	}
	if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	  break;
 }
break;

case 8:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
	if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	  fprintf(file_out_ptr, "y%d ", opt_tmp->hex);
	  i = inz;
	  return SUCCEEDED;
	}
	if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	  break;
 }
break;

case 9:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
	if (opt_tmp->op[x] == '?') {
	  y = i;
	  i = inz;
	  z = input_number();
	  inz = i;
	  i = y;
	  if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
	    return FAILED;
	  if (z == SUCCEEDED && (d < -32768 || d > 65535))
	    break;

	  for (x++; x < OP_SIZE_MAX; inz++, x++) {
	    if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	      if (z == SUCCEEDED)
					fprintf(file_out_ptr, "d%d y%d ", opt_tmp->hex, d);
	      else if (z == INPUT_NUMBER_ADDRESS_LABEL)
					fprintf(file_out_ptr, "k%d d%d M%s ", active_file_info_last->line_current, opt_tmp->hex, label);
	      else {
					fprintf(file_out_ptr, "d%d C%d ", opt_tmp->hex, latest_stack);
					/* let's configure the stack so that all label references inside are relative */
					stacks_tmp->relative_references = 1;
				}

	      i = inz;
	      return SUCCEEDED;
	    }
	    if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	      break;
	  }
	}
	if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	  break;
 }
break;

case 0xA:
if (xbit_size > 8 && opt_tmp->skip_xbit == 2)
	break;
for ( ; x < OP_SIZE_MAX; inz++, x++) {
	if (opt_tmp->op[x] == 'x') {
	  y = i;
	  i = inz;
	  z = input_number();
	  inz = i;
	  i = y;
	  if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
	    return FAILED;
	  if ((operand_hint == HINT_16BIT || operand_hint == HINT_24BIT) && opt_tmp->skip_xbit == 2)
	    break;
	  if (z == SUCCEEDED && (d > 255 || d < -128)) {
	    if (opt_tmp->skip_xbit == 2)
	      break;
	    print_error("Out of 8bit range.\n", ERROR_NUM);
	    return FAILED;
	  }

	  for (x++; x < OP_SIZE_MAX; inz++, x++) {
	    if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	      if (z == SUCCEEDED)
					fprintf(file_out_ptr, "d%d d%d ", opt_tmp->hex, d);
	      else if (z == INPUT_NUMBER_ADDRESS_LABEL)
					fprintf(file_out_ptr, "k%d d%d Q%s ", active_file_info_last->line_current, opt_tmp->hex, label);
	      else
					fprintf(file_out_ptr, "d%d c%d ", opt_tmp->hex, latest_stack);

	      i = inz;
	      return SUCCEEDED;
	    }
	    if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	      break;
	  }
	}
	if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	  break;
 }
break;
